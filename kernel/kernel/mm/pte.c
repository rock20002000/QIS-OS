#include "arch.h"
#include "debug.h"
#include "kernel.h"
#include "mm/buddy.h"

typedef struct
{
    paddr_t     paddr;
    uint32_t    flags;
} pte_map_params_t;

static error_t pte_map_cb(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    pte_map_params_t* p_map = (pte_map_params_t*)param;
    paddr_t paddr = p_map->paddr;

    if (NULL_PADDR == paddr)
    {
        uint32_t flags = map_flags_to_mem_flags(p_map->flags);

        paddr = alloc_pages(1, flags);

        if (NULL_PADDR == paddr)
            return ERR_NO_MEMORY;
        ms->pmem_pages++;
    }
#if 0
    else if (is_usable_paddr(paddr))
    {
        if (!reference_pages(paddr))
        {
            PANIC("reference_pages: %p, page:%x/%d",
                paddr, paddr_to_pn(paddr), paddr_to_page(paddr)->type);
        }
    }
#endif
    error_t err = arch_pte_map(ms->mmu, vaddr, paddr, p_map->flags);

    if (IS_ERROR(err))
    {
        if (paddr != p_map->paddr)
        {
            ms->pmem_pages--;
            free_pages(paddr);
        }
    }
    else if (NULL_PADDR != p_map->paddr)
        p_map->paddr += PAGE_SIZE;
    return err;
}

static error_t pte_unmap_cb(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    paddr_t paddr;
    error_t err = arch_pte_unmap(ms->mmu, vaddr, &paddr);

    if (IS_OK(err))
    {
        if ((bool)param && is_usable_paddr(paddr))
        {
            ms->pmem_pages -= paddr_pages(paddr);

            error_t err = free_pages(paddr);

            if (EOK != err)
                PANIC("pte_unmap_cb(%p) => free_pages(%p), page:%x err=%d",
                    vaddr, paddr, paddr_to_pn(paddr), err);
        }
    }
    return EOK; // skip ERR_NOT_FOUND
}

typedef struct
{
    struct mm_space*    ms;
    vaddr_t             vaddr;
    uint32_t            flags;
} pte_vmap_params_t;

static error_t pte_vmap_cb(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    pte_vmap_params_t* p_vmap = (pte_vmap_params_t*)param;
    paddr_t paddr = arch_pte_get(p_vmap->ms->mmu, p_vmap->vaddr);

    if (NULL_PADDR == paddr)
        return ERR_NOT_FOUND;

    error_t err = arch_pte_map(ms->mmu, vaddr, paddr, p_vmap->flags);

    if (IS_OK(err))
    {
        if (is_usable_paddr(paddr))
        {
            if (!reference_pages(paddr))
            {
                PANIC("reference_pages: %p, page:%x/%d",
                    paddr, paddr_to_pn(paddr), paddr_to_page(paddr)->type);
            }
            ms->pmem_pages++;
        }
        p_vmap->vaddr += PAGE_SIZE;
    }
    return err;
}

static error_t set_attr_ro(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    return arch_pte_set_ro(ms->mmu, vaddr);
}

static error_t set_attr_rw(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    return arch_pte_set_rw(ms->mmu, vaddr);
}

static error_t set_attr_user(struct mm_space* ms, vaddr_t vaddr, void* param)
{
    return arch_pte_set_user(ms->mmu, vaddr);
}


error_t pte_walk(struct mm_space* ms,
    vaddr_t vaddr, size_t size, pte_callback_t* cb, void* param)
{
    ASSERT(ms && ms->mmu);
    ASSERT(NULL_VADDR != vaddr);

    uint32_t pages = PFN_UP((vaddr & PAGE_MASK) + size);

    if (!pages)
        return EOK;

    error_t err = EOK;

    arch_pte_lock(ms->mmu);

    for (uint32_t i = 0; i < pages; i++)
    {
        err = cb(ms, vaddr, param);
        if (IS_ERROR(err))
            break;
        vaddr += PAGE_SIZE;
    }

    arch_pte_unlock(ms->mmu);

    return err;
}

error_t pte_alloc_map(struct mm_space* ms,
    vaddr_t vaddr, size_t size, paddr_t paddr, uint32_t flags)
{
    pte_map_params_t param = {paddr, flags};

    return pte_walk(ms, vaddr, size, pte_map_cb, &param);
}

error_t pte_unmap_free(struct mm_space* ms, vaddr_t vaddr, size_t size, bool is_free)
{
    return pte_walk(ms, vaddr, size, pte_unmap_cb, (void*)is_free);
}

error_t pte_alloc_map_vm(struct mm_space* dst_ms, vaddr_t dst_vaddr,
    struct mm_space* src_ms, vaddr_t src_vaddr, size_t size, uint32_t flags)
{
    pte_vmap_params_t param = {src_ms, src_vaddr, flags};

    arch_pte_lock(src_ms->mmu);

    error_t err = pte_walk(dst_ms, dst_vaddr, size, pte_vmap_cb, &param);

    arch_pte_unlock(src_ms->mmu);
    return err;
}

error_t pte_set_attr_ro(struct mm_space* ms, vaddr_t vaddr, size_t size)
{
    return pte_walk(ms, vaddr, size, set_attr_ro, NULL);
}

error_t pte_set_attr_rw(struct mm_space* ms, vaddr_t vaddr, size_t size)
{
    return pte_walk(ms, vaddr, size, set_attr_rw, NULL);
}

error_t pte_set_attr_user(struct mm_space* ms, vaddr_t vaddr, size_t size)
{
    return pte_walk(ms, vaddr, size, set_attr_user, NULL);
}
