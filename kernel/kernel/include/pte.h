#ifndef __PTE_H__
#define __PTE_H__

#include "arch_mm.h"

/**
 * Virtual address of memory map areas
 */
#define STACK_START_VADDR       (0)
#define STACK_END_VADDR         (PROC_START_VADDR - 1)
#define PROC_START_VADDR        (0x008048000)
#ifdef __LP64__
#define PROC_END_VADDR          (SHAREDLIBS_START_VADDR - 1)
#define SHAREDLIBS_START_VADDR  (0x0000000100000000)
#define SHAREDLIBS_END_VADDR    (SHAREDMEM_START_VADDR - 1)
#define SHAREDMEM_START_VADDR   (0x0000000180000000)
#define SHAREDMEM_END_VADDR     (0x0000008000000000 - 1)
#else   // 32bit
#define PROC_END_VADDR          (SHAREDLIBS_START_VADDR - 1)
#define SHAREDLIBS_START_VADDR  (0x60000000)
#define SHAREDLIBS_END_VADDR    (SHAREDMEM_START_VADDR - 1)
#define SHAREDMEM_START_VADDR   (0x80000000)
#define SHAREDMEM_END_VADDR     (KERNEL_START_VADDR - 1)
#endif  // __LP64__

struct mm_space;

/**
    @brief pte walk callback function
    @param ms current virtual memory space pointer
    @param vaddr virtual address
    @param param callback pointer
    @return error code, 'EOK' is successful
*/
typedef error_t (pte_callback_t)(struct mm_space* ms, vaddr_t vaddr, void* param);

/**
    @brief MMU travel
    @param ms current virtual memory space pointer
    @param vaddr start virtual address
    @param size travel size
    @param cb travel callback function
    @param param callback pointer
    @return error code, 'EOK' is successful
*/
extern error_t pte_walk(struct mm_space* ms,
    vaddr_t vaddr, size_t size, pte_callback_t* cb, void* param);


/**
    @brief Memory map a physical space to virtual space in ARCH MMU
    @param ms current virtual memory space pointer
    @param vaddr start virtual address
    @param size map size
    @param paddr start physical address
    @param flags memory map flags
    @return error code, 'EOK' is successful
*/
extern error_t pte_alloc_map(struct mm_space* ms,
    vaddr_t vaddr, size_t size, paddr_t paddr, uint32_t flags);

/**
    @brief Memory unmap the physical space from virtual space
    @param ms current virtual memory space pointer
    @param vaddr virtual address
    @param size unmap size
    @param is_free true for free physical page after umnmap
    @return error code, 'EOK' is successful
*/
extern error_t pte_unmap_free(struct mm_space* ms, vaddr_t vaddr, size_t size, bool is_free);

/**
    @brief Memory map travel for virtual address of other mmu
    @param dst_ms virtual memory space pointer for map to
    @param dst_vaddr destination virtual address
    @param src_ms virtual memory space pointer for map from
    @param src_vaddr source virtual address
    @param size map size
    @param flags destination memory map flags
    @return error code, 'EOK' is successful
*/
extern error_t pte_alloc_map_vm(struct mm_space* dst_ms, vaddr_t dst_vaddr,
    struct mm_space* src_ms, vaddr_t src_vaddr, size_t size, uint32_t flags);

/**
    @brief Set pte attr to readonly
    @param ms current virtual memory space pointer
    @param vaddr virtual address
    @param size size to set
    @return error code, 'EOK' is successful
*/
extern error_t pte_set_attr_ro(struct mm_space* ms, vaddr_t vaddr, size_t size);

/**
    @brief Set pte attr to read & write
    @param ms current virtual memory space pointer
    @param vaddr virtual address
    @param size size to set
    @return error code, 'EOK' is successful
*/
extern error_t pte_set_attr_rw(struct mm_space* ms, vaddr_t vaddr, size_t size);

/**
    @brief Set pte attr to user access
    @param ms current virtual memory space pointer
    @param vaddr virtual address
    @param size size to set
    @return error code, 'EOK' is successful
*/
extern error_t pte_set_attr_user(struct mm_space* ms, vaddr_t vaddr, size_t size);

/**
    @brief Get the physical address by virtual address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @return error code, 'EOK' is successful
*/
static inline paddr_t arch_vm_v2p_addr(struct arch_mmu* mmu, vaddr_t vaddr)
{
    return arch_pte_get(mmu, vaddr);
}

/**
    @brief Get the physical address by virtual address
    @param pml4 physical address of PML4 page
    @param vaddr virtual address
    @return error code, 'EOK' is successful
*/
static inline paddr_t arch_vm_v2p_by_pte(paddr_t pml4, vaddr_t vaddr)
{
    struct arch_mmu mmu;

    arch_pte_init(&mmu, pml4);

    return arch_vm_v2p_addr(&mmu, vaddr);
}

#endif
