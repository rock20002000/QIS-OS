#include <libc/include/string.h>
#include <libc/include/stdio.h>
#include <boot/include/kirk_image_def.h>

#include "kernel.h"
#include "task.h"
#include "arch_task.h"
#include "debug.h"
#include "cpu_common.h"


//	typedef void*(*start_rtn)(void*);
typedef void(*start_rtn)(void);


DYNARRAY_NEW(PROCESS_t*, process_vector);
PROCESS_t *g_proc_prp;

PROCESS_t g_ker_process;

void idle_handler(void)
{
    uint32_t idel_loops = 0;

    while(1)
    {
        KDBG("Idel task: %x\n", idel_loops++);
    }
}

static void load_appinfo(PROCESS_t* proc)
{
    const bsp_bootinfo_t* p_bootinfo = get_bsp_bootinfo();
    const application_hdr_t* src = (const application_hdr_t*)(p_bootinfo->mods_hdr_ptr + 1);
    vaddr_t vaddr = NULL_VADDR;
    paddr_t paddr = src[0].prog_hdr[0].lma;
    size_t size = p_bootinfo->load_end_addr  - paddr + 1;
    uint32_t flags = PROT_READ | MAP_PHYS | MAP_ANON | MAP_SHARED;

    // KDBG("App: %s, paddr:%p\n", src[0].name, src[0].prog_hdr[0].lma);

    if (EOK != vm_map(proc, &vaddr, paddr, &size, flags, NOFD, 0))
    {
        WARN("proc map modele data failed, paddr:%p+%x", paddr, size);
        return;
    }
    else
    {
        KDBG("load_appinfo data from:%p map to: %p + 0x%x\n", paddr, vaddr, size);
    }

    vaddr_t hdr_vaddr = NULL_VADDR;
    paddr_t hdr_paddr = ptr2paddr(src);
    uint8_t app_num = p_bootinfo->module_number - 1;
    size_t hdr_size = app_num * sizeof(application_hdr_t);

    if (EOK != vm_map(proc, &hdr_vaddr, hdr_paddr, &hdr_size, flags, NOFD, 0))
    {
        vm_unmap(proc, vaddr, size);
        WARN("proc map modele header failed, paddr:%p+%x", hdr_paddr, hdr_size);
        return;
    }
    // KDBG("load_appinfo header from:%p map to: %p + 0x%x\n", hdr_paddr, hdr_vaddr, hdr_size);

    vaddr_t cmdl_vaddr = PROC_CMDLINE_ADDR(proc->addr.rodata_end);
#ifdef KERNEL_VMAP_SIZE
    char* cmdline = kernel_map_user(proc, (const void*)cmdl_vaddr, PROC_CMDLINE_SIZE, KERNEL_MAP_FLAGS);

    if (!cmdline)
    {
        KWARN("load_appinfo: kernel_map_user cmdline failed!");
        return;
    }
    sprintf(cmdline, "%s -apps %d -header 0x%p -lma_off 0x%p", proc->name, app_num, hdr_vaddr, vaddr - paddr);

    KDBG("cmdline %p : %s\n", cmdl_vaddr, cmdline);
    kernel_unmap(cmdline, PROC_CMDLINE_SIZE);
#else
    paddr_t cmdl_paddr = arch_vm_v2p_addr(&proc->arch.mmu, cmdl_vaddr);

    if (NULL_PADDR == cmdl_paddr)
    {
        WARN("proc map cmdline failed, vaddr:%p", cmdl_vaddr);
        vm_unmap(proc, vaddr, size);
        vm_unmap(proc, hdr_vaddr, hdr_size);
        return;
    }
    char* cmdline = paddr2ptr(cmdl_paddr);

    sprintf(cmdline, "%s -apps %d -header 0x%x -lma_off 0x%x", proc->name, app_num, hdr_vaddr, vaddr - paddr);

    // KDBG("cmdline %p => %p : %s\n", cmdl_paddr, cmdl_vaddr, cmdline);
#endif
}

static error_t load_proc0(application_hdr_t* p_app_hdr, PROCESS_t* proc)
{
#define ELF_PF_X    (1)
#ifdef ELF_PF_X
 #define ELF_PF_W   (ELF_PF_X << 1)
#endif
    error_t err = EOK;

    for(uint8_t i = 0; i < p_app_hdr->ph_nmbers; i++)
    {
        ASSERT(p_app_hdr->prog_hdr[i].mem_size >= p_app_hdr->prog_hdr[i].file_size);

        /* Try to map each program section */
        uint32_t map_flags = 0;
        uint32_t attr = (uint32_t)p_app_hdr->prog_hdr[i].attr;
        vaddr_t vaddr = (vaddr_t)p_app_hdr->prog_hdr[i].vma;
        paddr_t paddr = p_app_hdr->prog_hdr[i].lma;
        size_t map_size = p_app_hdr->prog_hdr[i].file_size;

        if (attr & ELF_PF_X)
        {
            map_flags = MAP_FLAGS_CODE;
        }
        if (attr & ELF_PF_W)
        {
            map_flags = MAP_FLAGS_DATA;
        }

        if (map_size)
        {
            if (NULL_VADDR == vmalloc(proc, vaddr, &map_size, map_flags))
            {
                PANIC("proc%x:%s sec%x vaddr:0x%x + 0x%x, flags:%x, load from %p failed!",
                    proc->pid, proc->name, i, vaddr, map_size, map_flags, paddr);
                err = ERR_NO_MEMORY;
            }
            else
            {
                const char* src = (const char*)paddr2ptr(paddr);

                map_size = p_app_hdr->prog_hdr[i].file_size;
                copy_kernel_data(proc, (void*)vaddr, map_size, src);
                if (PROT_EXEC & map_flags)
                {
                    if (NULL_PADDR == proc->addr.rodata_end)
                    {
                        proc->addr.rodata_end = vaddr + map_size;
                    }
                    proc->addr.code_pages += PFN_UP(map_size);
                }
                else
                {
                    proc->addr.data_pages += PFN_UP(map_size);
                }
            }
        }
        else
        {
            map_size = p_app_hdr->prog_hdr[i].mem_size;
            map_flags = MAP_FLAGS_BSS;
            if (NULL_VADDR == vmalloc(proc, vaddr, &map_size, map_flags))
            {
                PANIC("proc%x:%s sec%x vaddr:0x%x + 0x%x, flags:%x, malloc failed!",
                    proc->pid, proc->name, i, vaddr, map_size, map_flags);
                err = ERR_NO_MEMORY;
            }
            else if (NULL_PADDR == proc->addr.bss_start)
            {
                proc->addr.bss_start = vaddr;
                proc->addr.stack_top = vaddr + MAIN_STACK_SIZE;
            }
        }
        if (err == EOK)
        {
            KDBG("proc%x:%s sec%x vaddr:0x%x + 0x%x, attr:%p, flags:%x\n", proc->pid,
                proc->name, i, vaddr, map_size, p_app_hdr->prog_hdr[i].attr, map_flags);
        }
        else
            break;
    }
    return err;
}

static error_t map_proc(PROCESS_t* proc, const proc_map_attr_t* pma)
{
    uint32_t i = 0;
    error_t err = EOK;

    for (; i < pma->nums && EOK == err; i++)
    {
        id_t id = NULL_ID;
        offset_t off = 0L;
        size_t size = pma->params[i].size;
        uint32_t flags = pma->params[i].flags;
        vaddr_t vaddr = pma->params[i].vaddr;

        if (!IS_MAP_FLAGS(pma->params[i].flags, MAP_ANON|MAP_LAZY))
        {
            id = pma->params[i].id;
            off = pma->params[i].off;
            flags |= MAP_VMEM;
        }
        err = vm_map(proc, &vaddr, NULL_PADDR, &size, flags, id, off);
        if (EOK != err)
        {
            WARN("proc%x:%s sec%x vaddr:0x%x + 0x%x, flags:%x, map failed!",
                proc->pid, proc->name, i, vaddr, pma->params[i].size, flags);
            break;
        }
        else
        {
            KDBG("proc%x:%s sec%x vaddr:0x%x + 0x%x, flags:%x\n",
                proc->pid, proc->name, i, vaddr, size, flags);
        }
        if (PROT_EXEC & pma->params[i].flags)
        {
            if (NULL_PADDR == proc->addr.rodata_end)
            {
                proc->addr.rodata_end = vaddr + pma->params[i].size;
            }
            proc->addr.code_pages += PFN_UP(size);
        }
        else if (MAP_ANON & pma->params[i].flags)
        {
            if (NULL_PADDR == proc->addr.bss_start)
            {
                proc->addr.bss_start = vaddr;
                proc->addr.stack_top = vaddr + MAIN_STACK_SIZE;
            }            
        }
        else
        {
            proc->addr.data_pages += PFN_UP(size);
        }
    }

    if (EOK != err)
    {
        for (uint32_t j = 0; j < i; j++)
            vm_unmap(proc, pma->params[j].vaddr, pma->params[j].size);
    }
    else
    {
        err = process_init_env(proc);
    }
    return err;
}

static void init_ker_proc(PROCESS_t* process)
{
    yieldlock_init(&process->lock);

    strcpy(process->name, "ker_proc");

    // process->pid = DYNARRAY_INSERT(process_vector, process);
    process->pid = NULL_ID / 2;
    process->parent = NULL;

    DYNARRAY_NULLIFY(process->threads);
    DYNARRAY_NULLIFY(process->children);

    // set current path is root;
    process->path = "/";

	// Create the idle thread for this cpu and attach it in proc
    cpuvar_ptr cpu_var = get_cpuvar();

    THREAD_t* idle_task = init_thread(&cpu_var->idle_task, "idle", idle_handler, MAX_PRIORITYS - 1, false);
#ifdef SCHED_IDLE_TASK
    id_pool_init(&process->thread_id_pool, MAX_THREAD_PER_PROC);

    process_bind_thread(process, idle_task);
#endif

    CURRENT_THREAD = idle_task;

    KDBG("idle_task: %p\n", idle_task);
}

/*build up proc with pid 0*/
void init_proc(void)
{
    DYNARRAY_NULLIFY(process_vector);

    //    for(int i=0;i<NUM_CPUS_MAX; i++){
    //    	kernel_stack_global[i] = NULL;	//	kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);
    //    }

    init_ker_proc(&g_ker_process);

    // Read the first app name in loadinfo,it must be 'proc'!
    const bsp_bootinfo_t* p_bootinfo = get_bsp_bootinfo();
    application_hdr_t* ptr_apphdr_proc = (application_hdr_t*)paddr2ptr((paddr_t)(p_bootinfo->mods_hdr_ptr));

    ASSERT(ptr_apphdr_proc->ph_nmbers);

    PROCESS_t* prp = process_create(ptr_apphdr_proc->name);

    g_proc_prp = prp;

    KINFO("Read app name from bootinfo: name = %s\n", prp->name);

    // Map this elf
    load_proc0(ptr_apphdr_proc, prp);

    load_appinfo(prp);

    process_init_env(prp);

    KINFO("Found proc:%p-%d '%s' entry @0x%p, pte=%p:%p\n", prp, prp->pid, prp->name,
        (vaddr_t)ptr_apphdr_proc->entry, arch_pte_get_entry(prp->ms.mmu), arch_pte_entry_ptr(prp->ms.mmu));
    // Load proc elf 
    // KINFO("Found proc:'%s' entry @0x%p, ", prp->name, ptr_apphdr_proc->entry);
    // KINFO("pte=%p\n", arch_pte_get_entry(prp->ms.mmu));

    vaddr_t entry = (vaddr_t)ptr_apphdr_proc->entry;
    THREAD_t* pt = msg_pthread_create(prp, NULL, NULL, (start_rtn)entry, NULL, 1);
    KINFO("main thread: %p, pid=%x\n", pt, prp->pid);
}

//	extern bool start_schedule;
error_t load_proc(const p_proc_create_attr_t pca)
{
    ASSERT(pca);

    PROCESS_t* proc = process_create(pca->name);

    proc->parent = getProcessWithPid(pca->parent);
    KINFO("Load app: name = %s, pid:%x, ms:%p, pte:%p\n",
        proc->name, proc->pid, &proc->ms, arch_pte_get_entry(proc->ms.mmu));

    error_t err = map_proc(proc, &pca->map);

    if (EOK != err)
    {
        // process_destroy(proc);
        return err;
    }

    KINFO("Found proc:%p-%d '%s' entry @0x%p, pte=%p:%p\n", proc, proc->pid, proc->name,
        (vaddr_t)pca->map.entry, arch_pte_get_entry(proc->ms.mmu), arch_pte_entry_ptr(proc->ms.mmu));

    THREAD_t* pt = msg_pthread_create(proc, NULL, NULL, (void *)pca->map.entry, NULL, pca->stat);
    KINFO("main thread: %p, pid=%x\n", pt, proc->pid);

    //	if (pca->stat)
    //start_schedule = true;

    return EOK;
}


//static inline
bool is_root_process(PROCESS_t* proc) { return 0 == proc->pid; }

//static inline
const char* process_path(PROCESS_t* proc) { return proc->path; }

//static inline
PROCESS_t* current_process(THREAD_t* current_thread) {
  if (!current_thread) current_thread = CURRENT_THREAD;
  if (!current_thread) return NULL;
  return current_thread->process;
}
