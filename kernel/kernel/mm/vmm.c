#include <platform/include/compiler.h>
#include <platform/include/pathobj.h>
#include "kernel.h"
#include "interrupt.h"
#include "task.h"
#include "arch_task.h"
#include "debug.h"
#include "buddy.h"

static const char* mm_area_name[] = 
{
    "NULL",
    "Stack",
    "App",
    "Libs",
    "Shared"
};

static const struct addr_area
{
    vaddr_t start;
    vaddr_t end;
} mm_space_addr[] =
{
    {STACK_START_VADDR, STACK_END_VADDR},
    {PROC_START_VADDR, PROC_END_VADDR},
    {SHAREDLIBS_START_VADDR, SHAREDLIBS_END_VADDR},
    {SHAREDMEM_START_VADDR, SHAREDMEM_END_VADDR}
};

static mm_space_t* kernel_space;


#define MAP_FLAGS_HEAP              (MAP_FLAGS_DATA | MAP_LAZY | MAP_FIXED)

/**
 * Is it can be merged in virtual space.
 */
#define MAP_IS_MERGABLE(flags)      IS_MAP_FLAGS(flags, MAP_FLAGS_HEAP)

/**
 * Is continuous physical memory, for DMA etc.
 */
#define MAP_IS_CONTIG(flags)        IS_MAP_FLAGS(flags, MAP_CONTIG)

static inline int map_flags_to_type(uint32_t flags)
{
    if (IS_MAP_FLAGS(flags, MAP_STACK))
        return VMM_STACK;
    if (IS_MAP_FLAGS(flags, MAP_ELF))
        return VMM_LIBS;
    return VMM_SHM; // for default
}

static int mm_area_type(vaddr_t vaddr)
{
    for (int i =0; i < ARRAY_SIZE(mm_space_addr); i++)
    {
        if (vaddr <= mm_space_addr[i].end)
            return i + VMM_STACK;
    }
#ifdef KERNEL_VMAP_SIZE
    if (vaddr != NULL_VADDR && vaddr >= KERNEL_VMAP_START)
        return VMM_SHM;
#endif
    return VMM_NULL;
}

static inline void mm_block_set(mm_block_t* mb, uint32_t flags, id_t id, offset_t off)
{
    mb->next = NULL;
    mb->flags = flags;
    mb->id = id;
    mb->off = off;
}
#define mm_block_reset(mb)  mm_block_set(mb, VMM_NULL, NOFD, 0)

static mm_block_t* mm_block_init(mm_block_t* mb,
    vaddr_t start, vaddr_t end, uint32_t flags, id_t id, offset_t off)
{
    if (!mb)
    {
        mb = kmalloc(sizeof(mm_block_t), KMEM);
        if (!mb)
            return NULL;
    }
    mb->start = PA_DOWN(start);
    mb->end = PA_UP(end) - 1;
    mm_block_reset(mb);

    return mb;
}

static void mm_block_info(const mm_block_t* mb)
{
    KINFO("  Block: %p ~ %p, flags:%x, id:%d, off:%p\n",
        mb->start, mb->end, mb->flags, mb->id, mb->off);
}

static bool mm_block_check_paddr(const mm_block_t* mb, paddr_t start, size_t size)
{
    if (!(mb->flags & MAP_PHYS))
        return false;
    if (start < mb->pmem)
        return false;
    if (mb->pmem + size > mb->pmem + (mb->end + 1 - mb->start))
        return false;
    return true;
}
static bool mm_block_check_vaddr(const mm_block_t* mb, vaddr_t start, size_t size)
{
    if (start < mb->start)
        return false;
    if (start + size - 1 > mb->end)
        return false;
    return true;
}

//static mm_block_t* mm_block_find_start(mm_block_t* mb, vaddr_t start)
//{
//    for (; mb && start >= mb->start; mb = mb->next)
//    {
//        if (start < mb->end)
//            return mb;
//    }
//
//    return NULL;
//}

static mm_block_t* mm_block_find_vaddr(mm_block_t* mb, vaddr_t start, size_t size)
{
    for (; mb && start >= mb->start; mb = mb->next)
    {
        if (mm_block_check_vaddr(mb, start, size))
            return mb;
    }

    return NULL;
}
static mm_block_t* mm_block_find_paddr(mm_block_t* mb, paddr_t start, size_t size)
{
    for (; mb && start >= mb->start; mb = mb->next)
    {
        if (mm_block_check_paddr(mb, start, size))
            return mb;
    }

    return NULL;
}

static void mm_block_free(mm_block_t* mb)
{
    kfree(mb);
}

static void mm_block_remove(mm_block_t** p_head, mm_block_t* mb)
{
    mm_block_t* prev = *p_head;

    if (!prev || !mb)
        return;
    
    if (mb == prev)
    {
        *p_head = prev->next;
        mb->next = NULL;
        return;
    }

    for (; prev->next; prev = prev->next)
    {
        if (mb == prev->next)
        {
            prev->next = mb->next;
            mb->next = NULL;
            return;
        }
    }
}

static vaddr_t mm_block_push(mm_block_t** p_head, mm_block_t* mb, bool merge)
{
    if (!mb)
        return NULL_VADDR;

    mm_block_t* head = *p_head;

    if (!head)
    {
        mb->next = NULL;
        *p_head = mb;
        return mb->start;
    }

    mm_block_t* prev = NULL, *next = head;
    
    for (; next; next = next->next)
    {
        if (mb->end < next->start)
        {
            break;
        }
        prev = next;
    }

    if (merge)
    {
        if (prev && prev->flags == mb->flags && prev->end + 1 == mb->start)
        {
            // KDBG("mm_block:%x %p~%p merge %p~%p\n",
            //     mb->flags, prev->start, prev->end, mb->start, mb->end);
            prev->end = mb->end;
            kfree(mb);
            return prev->start;
        }
        if (next && next->flags == mb->flags && mb->end + 1 == next->start)
        {
            // KDBG("mm_block:%x %p~%p merge %p~%p\n",
            //     mb->flags, mb->start, mb->end, next->start, next->end);
            next->start = mb->start;
            kfree(mb);
            return next->start;
        }
    }
    if (prev)
    {
        prev->next = mb;
    }
    else
    {
        *p_head = mb;
    }
    mb->next = next;
    return mb->start;
}

static void mm_area_init(mm_area_t* ma, vaddr_t base, vaddr_t top)
{
    ma->base = base;
    ma->top = top;
    ma->type = mm_area_type(base);
    spinlock_init(&ma->lock);
    list_init(&ma->list);
    ma->used = NULL;
    if (top > base)
    {
        ma->freed = mm_block_init(NULL, ma->base, ma->top, VMM_NULL, NOFD, 0);
    }
    else
    {
        ma->freed = NULL;
    }
}

static void mm_area_info(const mm_area_t* ma)
{
    int type = mm_area_type(ma->base);
    mm_block_t* mb = ma->used;

    KINFO("%s %p ~ %p, type:%d\n", mm_area_name[type], ma->base, ma->top, type);
    if (mb)
    {
        KINFO(" Used:\n");
    }
    for (; mb; mb = mb->next)
    {
        mm_block_info(mb);
    }

    mb = ma->freed;
    if (mb)
    {
        KINFO(" Freed:\n");
    }
    for (; mb; mb = mb->next)
    {
        mm_block_info(mb);
    }
}

#define MAX_VADDR   PA_DOWN(NULL_VADDR)
static mm_block_t* mm_area_get(mm_area_t* ma,
    vaddr_t* p_vaddr, size_t* p_size, uint32_t flags, id_t id, offset_t off)
{
    mm_block_t* next;
    mm_block_t* ret = NULL;
    size_t size = *p_size;
    vaddr_t start = *p_vaddr;
    offset_t vaddr_off = 0;
    bool merge= MAP_IS_MERGABLE(flags);

    if (start > MAX_VADDR)
        vaddr_off = start & PAGE_MASK;
    lock_acq(&ma->lock);
    for (mm_block_t*mb = ma->freed; mb; mb = next)
    {
        next = mb->next;
        if (start < MAX_VADDR)
        {
            if (start > mb->end)
                continue;
            if (start < mb->start || start + size - 1 > mb->end)
                break;
        }
        else
        {
            vaddr_t mb_start = PA_DOWN(mb->start) + vaddr_off;

            if (mb->start > mb_start)
                mb_start = PA_UP(mb->start) + vaddr_off;
            if (size > mb->end - mb_start + 1)
                continue;
            start = mb_start;
        }

        // align to page
        vaddr_t _start = PA_DOWN(start);
        vaddr_t _end = PA_UP(start + size);

        mm_block_remove(&ma->freed, mb);
        if (mb->start < _start)
        {
            mm_block_t* mb_head = mm_block_init(NULL, mb->start, _start, VMM_NULL, NOFD, 0);

            mb->start = start;
            mm_block_push(&ma->freed, mb_head, true);
        }
        if (_end < mb->end)
        {
            mm_block_t* mb_tail = mm_block_init(NULL, _end, mb->end, VMM_NULL, NOFD, 0);

            mb->end = _end - 1;
            mm_block_push(&ma->freed, mb_tail, true);
        }
        mm_block_set(mb, flags, id, off);
        mm_block_push(&ma->used, mb, merge);
        ret = mb;
        break;
    }
    lock_rel(&ma->lock);

    if (ret)
    {
        *p_vaddr = start;
        *p_size = ret->end - start + 1;
    }
    return ret;
}

static void mm_area_put(mm_area_t* ma, vaddr_t start, vaddr_t end)
{
    // align to page
    start = PA_DOWN(start);
    end = PA_UP(end) - 1;

    lock_acq(&ma->lock);
    for (mm_block_t* next, *mb = ma->used; mb; mb = next)
    {
        next = mb->next;
        if (start < mb->start)
            break;
        if (start > mb->end)
            continue;

        if (end > mb->end)
        {
            mm_area_info(ma);
            PANIC("mm_area_put start=%p, end=%p > %p", start, end, mb->end);
        }

        bool merge = MAP_IS_MERGABLE(mb->flags);

        mm_block_remove(&ma->used, mb);
        if (start > mb->start)
        {
            mm_block_t* mb_head = mm_block_init(NULL,
                mb->start, start - 1, mb->flags, mb->id, mb->off);

            mb->start = start;
            mm_block_push(&ma->used, mb_head, merge);
        }
        if (end < mb->end)
        {
            mm_block_t* mb_tail = mm_block_init(NULL,
                end + 1, mb->end, mb->flags, mb->id, mb->off);

            mb->end = end;
            mm_block_push(&ma->used, mb_tail, merge);
        }
        mm_block_reset(mb);
        mm_block_push(&ma->freed, mb, true);
    }
    lock_rel(&ma->lock);
}

static error_t vm_unmap_free(mm_space_t* ms, vaddr_t vaddr, size_t* p_size);
static void mm_area_free(mm_space_t* ms, mm_area_t* ma)
{
    lock_acq(&ma->lock);
    for (mm_block_t* mb = ma->used; mb; mb = mb->next)
    {
        size_t size = mb->end - mb->start + 1;

        vm_unmap_free(ms, mb->start, &size);
    }

    for (mm_block_t *next, *mb = ma->freed; mb; mb = next)
    {
        next = mb->next;
        mm_block_free(mb);
    }
    lock_rel(&ma->lock);
    kfree(ma);
}

static void mm_space_init(mm_space_t* ms, arch_mmu_t* mmu)
{
    // ms->last_pmb = NULL;
    ms->mmu = mmu;
    list_init(&ms->list);

    for (int i =0; i < ARRAY_SIZE(mm_space_addr); i++)
    {
        mm_area_t* ma = kmalloc(sizeof(*ma), KMEM);
        ASSERT(ma);
        mm_area_init(ma, mm_space_addr[i].start, mm_space_addr[i].end);
        list_push_back(&ms->list, &ma->list);
    }
    arch_pte_init(ms->mmu, NULL_PADDR);
    // Create page-dir for this process
    arch_pte_copy_kernel(ms->mmu, kernel_space->mmu);
    /* todo : we only map the trampoline page share the same vaddr in kernel&user-process */
}

static void mm_space_info(mm_space_t* ms)
{
    LIST_FOR_EACH(ma, &ms->list, mm_area_t, list)
    {
        mm_area_info(ma);
    }
}

void proc_ms_info(PROCESS_t* proc)
{
    KINFO("Process%u: %s, pte=%p:%p\n", proc->pid, proc->name,
        arch_pte_get_entry(proc->ms.mmu), arch_pte_entry_ptr(proc->ms.mmu));
    mm_space_info(&proc->ms);
}

mm_block_t* mm_space_find_used(mm_space_t* ms, vaddr_t start, size_t size)
{
    if (NULL_VADDR == start)
        return NULL;
    
    int type = mm_area_type(start);

    LIST_FOR_EACH(ma, &ms->list, mm_area_t, list)
    {
        if (type == ma->type)
            return mm_block_find_vaddr(ma->used, start, size);
    }

    return NULL;
}

static int mm_space_type(vaddr_t vaddr, uint32_t flags)
{
    int type = VMM_NULL;

    if (vaddr < MAX_VADDR)
    {
        type = mm_area_type(vaddr);
    }
    if (VMM_NULL == type)
        type = map_flags_to_type(flags);

    return type;
}

static mm_block_t* mm_space_get(mm_space_t* ms,
    vaddr_t* p_vaddr, size_t* p_size, uint32_t flags, id_t id, offset_t off)
{
    ASSERT(p_vaddr);

    int type = mm_space_type(*p_vaddr, flags);

    LIST_FOR_EACH(ma, &ms->list, mm_area_t, list)
    {
        if (type == ma->type)
        {
            mm_block_t* mb = mm_area_get(ma, p_vaddr, p_size, flags, id, off);

            if (mb)
                ms->vmem_pages += PFN_UP(mb->end - PA_DOWN(mb->start));
            return mb;
        }
    }
    // KDBG("mm_space_get(%p, 0x%x) = %d\n", *p_vaddr, flags, type);

    return NULL;
}

static void mm_space_put(mm_space_t* ms, vaddr_t start, vaddr_t end)
{
    ASSERT(end >= start);

    int type = mm_area_type(start);

    LIST_FOR_EACH(ma, &ms->list, mm_area_t, list)
    {
        if (type == ma->type)
        {
            ms->vmem_pages -= PFN_UP(end - PA_DOWN(start));
            return mm_area_put(ma, start, end);
        }
    }
}

static void mm_space_free(mm_space_t* ms)
{
    mm_area_t* ma;

    while ((ma = (mm_area_t*)list_pop_front(&ms->list)) != NULL)
    {
        mm_area_free(ms, ma);
    }
    ms->vmem_pages = 0;
    arch_pte_free(ms->mmu);
}

static void kernel_space_init(mm_space_t* ms, arch_mmu_t* mmu, syspage_t* p_syspage)
{
    kernel_space = ms;
    // memset(kernel_space, sizeof(kernel_space));
    kernel_space->mmu = mmu;
    arch_pte_init(mmu, p_syspage->pgdir[get_cpuvar()->cpu_id]);
    list_init(&kernel_space->list);
#ifdef KERNEL_VMAP_SIZE
    mm_area_t* kma = kmalloc(sizeof(*kma), KMEM);

    mm_area_init(kma, KERNEL_VMAP_START, KERNEL_MAX_VADDR);
    list_push_back(&kernel_space->list, &kma->list);
#endif
}

error_t vm_alloc_map(mm_space_t* ms, vaddr_t vaddr, size_t size, paddr_t paddr, mm_block_t* mb)
{
    ASSERT(mb);
    ASSERT(NULL_VADDR != vaddr);

    error_t err = pte_alloc_map(ms, vaddr, size, paddr, mb->flags);

    if (IS_ERROR(err))
    {
        WARN("vm_alloc_map vaddr=%p+%x, flags=0x%x, paddr=%p, err=%d, ms=%p",
            vaddr, size, mb->flags, paddr, err, ms);
        vm_unmap_free(ms, vaddr, &size);
    }
#if 0
    else
    {
        if (!MAP_IS_CONTIG(mb->flags) && mb->id != NOFD)
        {
            MSG_DATA msg;

            msg.iMsgType = MSG_READ;
            msg.iMsgLen  = len;
            msg.pszMsg = vaddr;

            err = msg_sys_read(mb->id, &msg);
            
            if (!err)
            {
                WARN("msg_sys_read(%d) %d bytes to %p", mb->id, len, vaddr);
            }
        }
    }
#endif
    return err;
}

static error_t vm_unmap_free(mm_space_t* ms, vaddr_t vaddr, size_t* p_size)
{
    error_t err = EOK;
    size_t size = *p_size;

    mm_block_t* mb = mm_space_find_used(ms, vaddr, 1);

    if (!mb)
    {
        WARN("vm_unmap_free vaddr=%p+0x%x not found", vaddr, size);
        return ERR_INVALID_ARG;
    }
    if (vaddr + size - 1 > mb->end)
    {
        WARN("vm_unmap_free end vaddr: %p > %p", vaddr + size - 1, mb->end);
        size = mb->end - vaddr + 1;
    }

    if (MAP_IS_CONTIG(mb->flags))
    {
        paddr_t paddr = arch_vm_v2p_addr(ms->mmu, vaddr);

        err = pte_unmap_free(ms, vaddr, size, false);
        if (IS_ERROR(err))
        {
            WARN("pte_unmap_free vaddr=%p+%x, paddr=%p, err:%d", vaddr, size, paddr, err);
        }
        else
        {
            ms->pmem_pages -= paddr_pages(paddr);
            err = free_pages(paddr);

            if (EOK != err)
            {
                KWARN("free_pages(%p) failed: %d\n", paddr, err);
            }
        }
    }
    else
    {
        err = pte_unmap_free(ms, vaddr, size, true);
        if (IS_ERROR(err))
        {
            WARN("pte_unmap_free vaddr=%p+%x, err:%d", vaddr, size, err);
        }
    }

    if (EOK == err)
    {
        if (ms->last_pmb && mm_block_check_vaddr(ms->last_pmb, vaddr, size))
            ms->last_pmb = NULL;
        mm_space_put(ms, vaddr, vaddr + size - 1);
        *p_size = size;
    }

    return err;
}

static error_t vm_unmap_ms(mm_space_t* ms, vaddr_t vaddr, size_t size)
{
    error_t err = EOK;

    for (size_t start = 0; start < size; )
    {
        size_t len = size - start;

        err = vm_unmap_free(ms, vaddr, &len);
        if (EOK != err)
        {
            WARN("vm_unmap vaddr=%p, size=0x%x", vaddr, size);
            break;
        }

        start += len;
        vaddr += len;
    }

    return err;
}

static error_t ms_verify_vaddr(mm_space_t* ms, vaddr_t vaddr, size_t size)
{
    mm_block_t* mb = mm_space_find_used(ms, vaddr, size);

    if (!mb)
    {
        KWARN("vaddr:%p + %x not found!\n", vaddr, size);
        return ERR_NOT_FOUND;
    }
    if (IS_MAP_FLAGS(mb->flags, MAP_LAZY))
    {
        for (size_t offs = 0; offs < size;)
        {
            size_t off = vaddr & PAGE_MASK;
            size_t len = size - offs < PAGE_SIZE - off ? size - offs : PAGE_SIZE - off;

            if (NULL_PADDR == arch_vm_v2p_addr(ms->mmu, vaddr))
            {
                error_t err = vm_alloc_map(ms, vaddr, len, NULL_PADDR, mb);

                if (IS_ERROR(err))
                    return err;
            }
            vaddr += len;
            offs += len;
        }
    }

    return EOK;
}

static paddr_t ms_get_paddr(mm_space_t* ms, vaddr_t vaddr)
{
    paddr_t paddr = arch_vm_v2p_addr(ms->mmu, vaddr);

    if (NULL_PADDR == paddr)
    {
        mm_block_t* mb = mm_space_find_used(ms, vaddr, 1);

        if (mb && IS_MAP_FLAGS(mb->flags, MAP_LAZY))
        {
            size_t size = PAGE_SIZE - (vaddr & PAGE_MASK);

            if (EOK == vm_alloc_map(ms, vaddr, size, NULL_PADDR, mb))
                paddr = arch_vm_v2p_addr(ms->mmu, vaddr);
        }
    }

    return paddr;
}

static void* map_user(PROCESS_t* proc, const void* user_data, size_t size, uint32_t flags)
{
    size_t len = size;
    vaddr_t vaddr = (vaddr_t)user_data;
    offset_t off = vaddr & PAGE_MASK;
    vaddr_t kvaddr = MAX_VADDR + off;

    mm_block_t* mb = mm_space_get(kernel_space, &kvaddr, &len, flags|MAP_VMEM, proc->pid, vaddr);
    if (!mb)
    {
        WARN("mm_space_get from kernel, size=0x%x, flags=0x%x", size, flags);
        return NULL;
    }

    error_t err = pte_alloc_map_vm(kernel_space,
        kvaddr, &proc->ms, vaddr, size, mb->flags);

    if (IS_ERROR(err))
    {
        vm_unmap_ms(kernel_space, kvaddr, size);
        return NULL;
    }
    return (void*)kvaddr;
}

static error_t copy_user(PROCESS_t* proc, const void* user_data, size_t size, void* kernel_data)
{
#ifdef KERNEL_VMAP_SIZE
    const void* src = kernel_map_user(proc, user_data, size, KERNEL_MAP_RFLAGS);

    if (!src)
        return ERR_NO_MEMORY;
    memcpy(kernel_data, src, size);
    kernel_unmap(src, size);
#else
    vaddr_t vaddr = (vaddr_t)user_data;
    unsigned char* dst = (unsigned char*)kernel_data;

    for (size_t offs = 0; offs < size;)
    {
        size_t off = vaddr & PAGE_MASK;
        size_t len = size - offs < PAGE_SIZE - off ? size - offs : PAGE_SIZE - off;
        paddr_t paddr = arch_vm_v2p_addr(proc->ms.mmu, vaddr);

        if (NULL_PADDR == paddr)
        {
            WARN("Proc%d get paddr from vaddr:%p failed", proc->pid, vaddr);
            return ERR_NOT_FOUND;
        }
        memcpy(dst, paddr2ptr(paddr), len);
        vaddr += len;
        dst += len;
        offs += len;
    }
#endif
    return EOK;
}

vdso_data_t* p_vdso_data;
__section(".data.vdso.datapage") vdso_data_t vdso_shared_data;
static size_t vdso_data_size, vdso_text_size;
static paddr_t pmem_vdso_data, pmem_vdso_text;
static void vdso_init(syspage_t* p_syspage)
{
extern char __vdso_data_start;
extern char __vdso_data_end;
extern char __vdso_text_start;
extern char __vdso_text_end;

#ifndef ELF_HEAD_LEN
    #define ELF_HEAD "\177ELF"
    #define ELF_HEAD_LEN 4
#endif
    p_vdso_data = NULL;
    vdso_data_size = &__vdso_data_end - &__vdso_data_start;
    vdso_text_size = &__vdso_text_end - &__vdso_text_start;
    if (!vdso_data_size || !vdso_text_size)
        return;
#ifdef VDSO_COPY
    pmem_vdso_data = alloc_pages(PFN_UP(vdso_data_size), PMEM);
    if (NULL_PADDR != pmem_vdso_data)
    {
        pmem_vdso_text = alloc_pages(PFN_UP(vdso_text_size), PMEM);
        if (NULL_PADDR != pmem_vdso_text)
        {
            memcpy(paddr2ptr(pmem_vdso_text), &__vdso_text_start, vdso_text_size);
            memcpy(&__vdso_text_start, ELF_HEAD, ELF_HEAD_LEN);
            p_vdso_data = (vdso_data_t*)paddr2ptr(pmem_vdso_data);
            memcpy(p_vdso_data, &__vdso_data_start, vdso_data_size);
            if (p_syspage->systime.get_ns)
            {
                memcpy(&p_vdso_data->systime, &p_syspage->systime, sizeof(p_vdso_data->systime));
                memcpy((char*)paddr2ptr(pmem_vdso_text) + vdso_text_size, p_syspage->systime.get_cycles, p_syspage->systime.code_size);
                p_vdso_data->systime.get_cycles = (size_t (*)(const struct vdso_systime*))((void*)vdso_text_size);
                vdso_text_size += p_syspage->systime.code_size;
            }
            KDBG("vdso_data paddr=%p, vdso_text paddr=%p\n", pmem_vdso_data, pmem_vdso_text);
            return;
        }
        else
        {
            free_pages(pmem_vdso_data);
            PANIC("vdso alloc text: %x failed!", vdso_text_size);
        }
    }
    else
    {
        PANIC("vdso alloc data: %x failed!", vdso_data_size);
    }
    vdso_data_size = 0;
    vdso_text_size = 0;
#else
    size_t vsdo_size = &__vdso_text_end - &__vdso_data_start;

    memcpy(&__vdso_text_start, ELF_HEAD, ELF_HEAD_LEN);
    pmem_vdso_data = kptr2paddr(&__vdso_data_start);
    pmem_vdso_text = kptr2paddr(&__vdso_text_start);
    pte_set_attr_user(kernel_space, (vaddr_t)&__vdso_data_start, vsdo_size);
    p_vdso_data = (vdso_data_t*)paddr2ptr(pmem_vdso_data);
    if (p_syspage->systime.get_cycles && vdso_text_size + p_syspage->systime.code_size <= PA_UP(vdso_text_size))
    {
        memcpy(&p_vdso_data->systime, &p_syspage->systime, sizeof(p_vdso_data->systime));
        memcpy(&__vdso_text_start + vdso_text_size, p_syspage->systime.get_cycles, p_syspage->systime.code_size);
        p_vdso_data->systime.get_cycles = (size_t (*)(const struct vdso_systime*))((void*)vdso_text_size);
        vdso_text_size += p_syspage->systime.code_size;
    }
    KDBG("vdso_data=%p:%p + %x, vdso_text=%p:%p + %x\n",
        &__vdso_data_start, pmem_vdso_data, vdso_data_size, &__vdso_text_start, pmem_vdso_text, vdso_text_size);
#endif
    if (p_vdso_data && p_vdso_data->systime.timer_freq)
        p_vdso_data->cycles_per_tick = p_vdso_data->systime.cycles_per_sec / p_vdso_data->systime.timer_freq;
}

static bool is_vdso(paddr_t paddr, size_t size)
{
    if (vdso_data_size && paddr >= pmem_vdso_data && paddr+size <= pmem_vdso_data+vdso_data_size)
        return true;
    if (vdso_text_size && paddr >= pmem_vdso_text && paddr+size <= pmem_vdso_text+vdso_text_size)
        return true;
}

static void vdso_map(PROCESS_t* proc)
{
#define VSDO_DATA_FLAGS (MAP_FLAGS_RO)
#define VSDO_TEXT_FLAGS (MAP_FLAGS_CODE)
    vaddr_t vdso_data = NULL_VADDR;
    vaddr_t vdso_text = NULL_VADDR;
    paddr_t start = pmem_vdso_data;
    paddr_t size = vdso_data_size;

    if (!vdso_data_size || !vdso_text_size)
    {
        // WARN("No VDSO!");
        return;
    }

    if (EOK != vm_map(proc, &vdso_data, start, &size, VSDO_DATA_FLAGS, NOFD, 0))
    {
        WARN("map vdso data: %p + %x failed", start, size);
    }
    proc->addr.vdso_data = vdso_data;
    start = pmem_vdso_text;
    size = vdso_text_size;
    if (EOK != vm_map(proc, &vdso_text, start, &size, VSDO_TEXT_FLAGS, NOFD, 0))
    {
        WARN("map vdso text: %p + %x failed", start, size);
    }
    proc->addr.vdso_text = vdso_text;
    KDBG("Process%d, vdso_data=%p, vdso_text=%p\n", proc->pid, vdso_data, vdso_text);
}

void vmm_init(PROCESS_t* ker_proc, syspage_t* p_syspage)
{
    kernel_space_init(&ker_proc->ms, &ker_proc->arch.mmu, p_syspage);
    vdso_init(p_syspage);
}

void vm_init(PROCESS_t* proc)
{
    mm_space_init(&proc->ms, &proc->arch.mmu);
    proc->addr.rodata_end = NULL_PADDR;
    proc->addr.bss_start = NULL_PADDR;
    proc->addr.stack_top = NULL_PADDR;
    vdso_map(proc);
}

error_t vm_map(PROCESS_t* proc, vaddr_t* p_vaddr,
    paddr_t paddr, size_t* p_size, uint32_t flags, id_t id, offset_t off)
{
    if (!proc || !p_vaddr || !p_size || !*p_size)
        return ERR_INVALID_ARG;

    bool vspace = false;
    size_t size = *p_size;
    mm_block_t* mb = NULL;
    PROCESS_t* shm_proc = NULL;
    mm_block_t* shm_mb = NULL;
    vaddr_t shm_vaddr = NULL_VADDR;
    vaddr_t vaddr_in = *p_vaddr;
    offset_t vaddr_off = 0L;

    if (IS_MAP_FLAGS(flags, MAP_PHYS) && off && off != NULL_VADDR)
    {
        if (!is_io_paddr(off))
        {
            WARN("Invalid IO address: %p", off);
            return ERR_INVALID_ARG;
        }
        paddr = (paddr_t)off;
    }
    else if (IS_MAP_FLAGS(flags, MAP_SHARED) && NOFD != id && !off) // shm object
    {
        path_object_t* obj = fd_get(proc, (int)id);

        if (!obj)
        {
            WARN("shm object fd: %d not found", id);
            return ERR_INVALID_ARG;
        }

        shm_block_t* shmb = (shm_block_t*)obj->handle;

        if (!shmb)
        {
            WARN("shm_block not found in object: %p", obj);
            return ERR_NOT_FOUND;
        }
        paddr = shmb->paddr;
        if (size > shmb->size)
            size = *p_size = shmb->size;
        // KDBG("MAP_SHARED = %p:%p, page:%x/%d\n", shmb, paddr, paddr_to_pn(paddr), paddr_to_page(paddr)->type);
    }

    if (IS_MAP_FLAGS(flags, MAP_VMEM))  // shared memory of other process
    {
        if (NULL_PADDR != paddr || id == NOFD || off > SHAREDMEM_END_VADDR)
        {
            WARN("shared map invalid args: paddr=%p, pid=%d, vaddr=%p, flags=%x", paddr, id, off, flags);
            return ERR_INVALID_ARG;
        }
        shm_proc = getProcessWithPid((pid_t)id);
        if (!shm_proc)
        {
            WARN("Can't find shm process%d", id);
            return ERR_INVALID_ARG;
        }
        shm_vaddr = (vaddr_t)off;
        shm_mb = mm_space_find_used(&shm_proc->ms, shm_vaddr, size);
        if (!shm_mb)
        {
            WARN("shm process%d vaddr:%p not found", id, off);
            return ERR_INVALID_ARG;
        }
        if (!IS_MAP_FLAGS(shm_mb->flags, MAP_SHARED))
        {
            WARN("shm process%d vaddr:%p can't be shared", id, off);
            return ERR_NOT_PERMITTED;
        }
        if (shm_mb->end - shm_vaddr + 1 < size)
        {
            WARN("shared map size %p + %x > %p", shm_vaddr, size, shm_mb->end);
            proc_ms_info(shm_proc);
            return ERR_INVALID_ARG;
        }
        if (!size)
            size = shm_mb->end - shm_vaddr;
        flags &= ~MAP_VMEM;
        if (!flags)
            flags = shm_mb->flags;
        vaddr_off = shm_vaddr & PAGE_MASK;
    }
    else if (NULL_PADDR != paddr)
        vaddr_off = paddr & PAGE_MASK;

    // get virtual space
    if (NULL_VADDR != *p_vaddr)
    {
        if ((mb = mm_space_find_used(&proc->ms, *p_vaddr, size)))
        {
            ASSERT(mb->flags == flags);
            ASSERT(mb->id == id);
            ASSERT(mb->off == off);
        }
    }

    error_t err = EOK;

    if (!mb)
    {
        if (NULL_VADDR == *p_vaddr)
            *p_vaddr = MAX_VADDR + vaddr_off;
        mb = mm_space_get(&proc->ms, p_vaddr, p_size, flags, id, off);
        if (!mb)
        {
            WARN("mm_space_get pid=%x, size=0x%x, flags=0x%x", proc->pid, *p_size, flags);
            proc_ms_info(proc);
            return ERR_NO_MEMORY;
        }
        if (IS_MAP_FLAGS(flags, MAP_FIXED) && vaddr_in != NULL_VADDR && vaddr_in != *p_vaddr)
        {
            WARN("mm_space_get pid=%x, MAP_FIXED get vaddr %p != %p", proc->pid, *p_vaddr, vaddr_in);
            err = ERR_INVALID_ARG;
            goto map_end;
        }
        vaddr_off = *p_vaddr & PAGE_MASK;
        vspace = true;
    }

    if (shm_mb)
    {
        err = pte_alloc_map_vm(&proc->ms, *p_vaddr,
            &shm_proc->ms, shm_vaddr, size, mb->flags);
        goto map_end;
    }

    if (IS_MAP_FLAGS(flags, MAP_PHYS))
    {
        if (NULL_PADDR != paddr)
        {
            mb->pmem = paddr;
        }
        else if (!IS_MAP_FLAGS(flags, MAP_ANON))
        {
            WARN("Can't map physical memory address: %p", paddr);
            err = ERR_INVALID_ARG;
            goto map_end;
        }
        else
        {
            uint32_t mem_flags = map_flags_to_mem_flags(flags);
            uint32_t pages = PFN_UP(vaddr_off + size);

            paddr = alloc_pages(pages, mem_flags);
            if (NULL_PADDR == paddr)
            {
                PANIC("alloc_pages pid=%x, size=0x%x, flags=0x%x", proc->pid, size, flags);
                err = ERR_NO_MEMORY;
                goto map_end;
            }
            proc->ms.pmem_pages += pages;
            // KDBG("vm_map: %p ~ %p => %p, page:%x/%d",
            //     mb->start, mb->end, paddr, paddr_to_pn(paddr), paddr_to_page(paddr)->type);
            paddr += vaddr_off;
            mb->pmem = paddr;
            mb->flags |= MAP_CONTIG;
        }
    }
    else if (NULL_PADDR != paddr)
    {
        if (is_usable_paddr(paddr))
        {
            if (!reference_pages(paddr))
            {
                PANIC("reference_pages: %p, page:%x/%d",
                    paddr, paddr_to_pn(paddr), paddr_to_page(paddr)->type);
            }
            mb->flags |= MAP_CONTIG;
        }
        else if (!is_vdso(paddr, size) && !(!(flags & PROT_WRITE) && is_rom_paddr(paddr)))
        {
            WARN("Invalid physical memory address: %p", paddr);
            err = ERR_INVALID_ARG;
            goto map_end;
        }
    }
    if (NULL_PADDR != paddr)
    {
        vaddr_t vaddr = PA_DOWN(*p_vaddr);

        vaddr += paddr & PAGE_MASK;   // align to paddr
        *p_vaddr = vaddr;
    }
    if (!IS_MAP_FLAGS(flags, MAP_LAZY))
    {
        err = vm_alloc_map(&proc->ms, *p_vaddr, size, paddr, mb);
    }
map_end:
    if (EOK != err)
    {
        if (is_usable_paddr(paddr))
        {
            proc->ms.pmem_pages -= paddr_pages(paddr);
            free_pages(paddr);
        }
        if (vspace)
        {
            vaddr_t vaddr = *p_vaddr;

            mm_space_put(&proc->ms, vaddr, vaddr + size - 1);
        }
    }
    else
    {
        if (IS_MAP_FLAGS(mb->flags, MAP_PHYS))
        {
            proc->ms.last_pmb = mb;
        }
    }
    return err;
}

error_t vm_unmap(PROCESS_t* proc, vaddr_t vaddr, size_t size)
{
    if (!proc || vaddr == NULL_VADDR || !size)
        return ERR_INVALID_ARG;

    return vm_unmap_ms(&proc->ms, vaddr, size);
}

error_t vm_offset(PROCESS_t* proc, vaddr_t vaddr, size_t size, shm_block_t* shmb)
{
    if (!proc || vaddr == NULL_VADDR || !size)
        return ERR_INVALID_ARG;

    mm_block_t* mb = mm_space_find_used(&proc->ms, vaddr, size);

    if (!mb)
        return ERR_NOT_FOUND;

    if (IS_MAP_FLAGS(mb->flags, MAP_PHYS))
    {
        shmb->paddr = mb->pmem + (vaddr - mb->start);
        shmb->size = mb->end - vaddr + 1;
    }
    else
    {
        shmb->paddr = arch_vm_v2p_addr(proc->ms.mmu, vaddr);
        shmb->size = vaddr & PAGE_MASK;
    }
    shmb->flags = mb->flags;

    return EOK;
}

void vm_free(PROCESS_t* proc)
{
    if (!proc)
        return;
    mm_space_free(&proc->ms);
}

#ifdef USER_INCLUDE_KERNEL_PTE
void* map_from_user(PROCESS_t* proc, const void* user_data, size_t size, uint32_t flags)
{
#ifdef KERNEL_VMAP_SIZE
    if (!size || !user_data)
        return NULL;
    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return NULL;

    return map_user(proc, user_data, size, flags);
#else
    return NULL;
#endif
}
#endif

void* kernel_map_user(PROCESS_t* proc, const void* user_data, size_t size, uint32_t flags)
{
#ifdef KERNEL_VMAP_SIZE
    if (!size || !user_data)
        return NULL;
    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return NULL;

#ifdef USER_INCLUDE_KERNEL_PTE
    if (arch_pte_get_entry(proc->ms.mmu) == arch_pte_read())
        return (void *)user_data;
#endif
    return map_user(proc, user_data, size, flags);
#else
    return NULL;
#endif
}

error_t kernel_unmap(const void* ptr, size_t size)
{
#ifdef KERNEL_VMAP_SIZE
    if (!size || !ptr)
        return ERR_INVALID_ARG;
    
#ifdef USER_INCLUDE_KERNEL_PTE
    // PROCESS_t* proc = current_process(NULL);

    // if (proc && arch_pte_get_entry(proc->ms.mmu) == arch_pte_read())
    //     vm_unmap_ms(&proc->ms, (vaddr_t)ptr, size);
    if ((vaddr_t)ptr < KERNEL_START_VADDR)
        return ERR_INVALID_ARG;
#endif
    // KDBG("kernel_unmap(%p, %x)\n", ptr, size);
    return vm_unmap_ms(kernel_space, (vaddr_t)ptr, size);
#else
    return ERR_NOT_SUPPORT;
#endif
}

paddr_t user_get_paddr(PROCESS_t* proc, vaddr_t vaddr)
{
    paddr_t paddr = ms_get_paddr(&proc->ms, vaddr);

    if (NULL_PADDR == paddr)
    {
        WARN("Proc%d get paddr from vaddr:%p failed", proc->pid, vaddr);
    }
    return paddr;
}

error_t copy_from_user(PROCESS_t* proc, const void* user_data, size_t size, void* kernel_data)
{
    if (!size)
        return ERR_INVALID_ARG;

    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return ERR_INVALID_TASK;

    return copy_user(proc, user_data, size, kernel_data);
}

void* copy_user_data(PROCESS_t* proc, const void* user_data, size_t size)
{
    if (!size)
        return NULL;

    if (!proc)
        proc = current_process(NULL);

    if (!proc)
        return NULL;

    void* ptr = kmalloc(size, KMEM);

    if (!ptr)
        return NULL;

    if (EOK != copy_user(proc, user_data, size, ptr))
    {
        kfree(ptr);
        ptr = NULL;
    }

    return ptr;
}

error_t copy_kernel_data(PROCESS_t* proc, void* user_data, size_t size, const void* kernel_data)
{
    if (!size)
        return EOK;

    if (!user_data)
        return ERR_INVALID_TASK;

    if (!proc)
        proc = current_process(NULL);
    if (!proc)
        return ERR_INVALID_TASK;

#ifdef KERNEL_VMAP_SIZE
    error_t err = ms_verify_vaddr(&proc->ms, (vaddr_t)user_data, size);

    if (EOK != err)
        return err;

    void* dst = kernel_map_user(proc, user_data, size, KERNEL_MAP_WFLAGS);

    if (!dst)
        return ERR_NO_MEMORY;

    memcpy(dst, kernel_data, size);
    kernel_unmap(dst, size);
#else
    bool remap = false;
    vaddr_t vaddr = (vaddr_t)user_data;
    unsigned char* src = (unsigned char*)kernel_data;

    for (size_t offs = 0; offs < size;)
    {
        size_t off = vaddr & PAGE_MASK;
        size_t len = size - offs < PAGE_SIZE - off ? size - offs : PAGE_SIZE - off;
        paddr_t paddr = ms_get_paddr(&proc->ms, vaddr);

        if (NULL_PADDR == paddr)
        {
            WARN("Proc%d get paddr from vaddr:%p failed", proc->pid, vaddr);
            return ERR_NO_MEMORY;
        }
        memcpy(paddr2ptr(paddr), src, len);
        vaddr += len;
        src += len;
        offs += len;
        remap = false;
    }
#endif
    return EOK;
}

error_t user_copy_user(PROCESS_t* dst_proc, void* dst_data, size_t size, PROCESS_t* src_proc, const void* src_data)
{
    if (!size)
        return EOK;

    if (!src_proc || !src_data)
        return ERR_INVALID_ARG;
    if (!dst_proc || !dst_data)
        return ERR_INVALID_ARG;

#ifdef KERNEL_VMAP_SIZE
    error_t err = ms_verify_vaddr(&src_proc->ms, (vaddr_t)src_data, size);

    if (EOK != err)
        return err;

    const void* src = kernel_map_user(src_proc, src_data, size, KERNEL_MAP_RFLAGS);

    if (!src)
        return ERR_NO_MEMORY;

    void* dst = kernel_map_user(dst_proc, dst_data, size, KERNEL_MAP_WFLAGS);

    if (!dst)
    {
        kernel_unmap(src, size);
        return ERR_NO_MEMORY;
    }

    memcpy(dst, src, size);

    kernel_unmap(dst, size);
    kernel_unmap(src, size);
#else
    vaddr_t src_vaddr = (vaddr_t)src_data;
    vaddr_t dst_vaddr = (vaddr_t)dst_data;
    uint32_t roff = src_vaddr & PAGE_MASK;
    uint32_t woff = dst_vaddr & PAGE_MASK;

    for (size_t len = 0; len < size; )
    {
        size_t rlen = size < PAGE_SIZE - roff ? size : PAGE_SIZE - roff;
        paddr_t raddr = arch_vm_v2p_addr(src_proc->ms.mmu, src_vaddr);

        if (NULL_PADDR == raddr)
        {
            WARN("Proc%d get paddr from vaddr:%p failed", src_proc->pid, src_vaddr);
            return ERR_INVALID_ARG;
        }

        for (size_t bytes = 0; bytes < rlen; )
        {
            size_t wlen = rlen < PAGE_SIZE - woff ? rlen : PAGE_SIZE - woff;
            paddr_t waddr = ms_get_paddr(&dst_proc->ms, dst_vaddr);

            if (NULL_PADDR == waddr)
            {
                WARN("Proc%d get paddr from vaddr:%p failed", dst_proc->pid, dst_vaddr);
                return ERR_NO_MEMORY;
            }
            memcpy(paddr2ptr(waddr), paddr2ptr(raddr), wlen);
            dst_vaddr += wlen;
            raddr += wlen;
            woff += wlen;
            bytes += wlen;
        };
        src_vaddr += rlen;
        len += rlen;
        roff = 0;
        woff &= PAGE_MASK;
    }
#endif
    return EOK;
}

error_t shm_create(path_object_t* p_obj)
{
    void* shmb = kmallocz(sizeof(shm_block_t));

    if (!shmb)
        return ERR_NO_MEMORY;
    p_obj->handle = (handle_t)shmb;
    return EOK;
}

error_t shm_delete(path_object_t* p_obj)
{
    if (NULL_HANDLE != p_obj->handle)
    {
        kfree((void*)p_obj->handle);
        p_obj->handle = NULL_HANDLE;
    }
    return EOK;
}

error_t shm_ioctl(path_object_t* p_obj, const void* param, size_t size)
{
    if (!param || size != sizeof(shm_block_t) || PATH_OBJECT_SHM != p_obj->type)
        return ERR_INVALID_ARG;
    if (NULL_HANDLE == p_obj->handle)
        return ERR_EMPTY;

    const shm_block_t* shmb = (shm_block_t*)param;

    if (!(SHMCTL_PHYS & shmb->flags))
        return ERR_NOT_ACCEPTABLE;

    mm_block_t* mb = NULL;
    PROCESS_t* proc = current_process(NULL);

    if (!proc)
        return ERR_INVALID_TASK;

    if (proc->ms.last_pmb && mm_block_check_paddr(proc->ms.last_pmb, shmb->paddr, shmb->size))
    {
        mb = proc->ms.last_pmb;
    }
    if (!mb)
    {
        LIST_FOR_EACH(ma, &proc->ms.list, mm_area_t, list)
        {
            if (VMM_SHM == ma->type)
                mb = mm_block_find_paddr(ma->used, shmb->paddr, shmb->size);
        }
    }
    if (!mb)
        return ERR_NOT_FOUND;
    // KDBG("shm_ioctl: %p:%p = %p + %x\n", p_obj, p_obj->handle, shmb->paddr, shmb->size);
    memcpy((void*)p_obj->handle, shmb, sizeof(shm_block_t));
    return EOK;
}
