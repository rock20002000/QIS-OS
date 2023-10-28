#ifndef __ARM_PAGE_H__
#define __ARM_PAGE_H__

#include <stdint.h>
#include <arch_syspage.h>
#include "locks.h"
#include "page.h"

// #define USER_INCLUDE_KERNEL_PTE

#ifndef KERNEL_START_VADDR
 #define KERNEL_START_VADDR     (0xC0000000)
#endif  // KERNEL_START_VADDR
#define VECTOR_MAPSIZE          (0x10000) // reserved for vector
#define KERNEL_MAX_VADDR        (-1UL - VECTOR_MAPSIZE)
// #define KERNEL_MAX_VADDR        (-1UL - ALIGN_UP(VECTOR_MAPSIZE, ARM_SCSIZE))
#define KERNEL_VMAP_SIZE        (0x08000000 - VECTOR_MAPSIZE)

#ifdef KERNEL_VMAP_SIZE
 #define KERNEL_VMAP_START      (KERNEL_MAX_VADDR - KERNEL_VMAP_SIZE + 1)
 #define KERNEL_END_VADDR       (KERNEL_VMAP_START - 1)
#else
 #define KERNEL_END_VADDR       KERNEL_MAX_VADDR
#endif
#define KERNEL_VADDR_SIZE       (KERNEL_END_VADDR - KERNEL_START_VADDR + 1)

typedef struct arch_mmu {
  spinlock_t lock;
  paddr_t* pde;
} arch_mmu_t;
typedef struct
{
    kernel_hdr_t* ker_hdr_ptr;
    application_hdr_t* mods_hdr_ptr;
    paddr_t load_end_addr;
    memmap_entry_t* memmap;
    uint16_t memmap_number;
    uint16_t module_number;
} bsp_bootinfo_t;

/**
    @brief Get boot info in bsp
    @return module info pointer
*/
extern const bsp_bootinfo_t* get_bsp_bootinfo(void);

extern paddr_t kernel_paddr_start, kernel_paddr_end;
extern size_t kernel_p2v_offset;
static inline bool is_kernel_paddr(paddr_t paddr)
{
  return (paddr >= kernel_paddr_start) && (paddr <= kernel_paddr_end);
}

/**
    @brief Check the virtual address is in kernel space
    @param vaddr virtual memory address
    @return True for kernel space
*/
static inline bool is_kernel_vaddr(vaddr_t vaddr)
{
  return (vaddr >= (kernel_paddr_start + kernel_p2v_offset))
    && (vaddr <= (kernel_paddr_end + kernel_p2v_offset));
}

/**
    @brief Physical address convert to pointer
    @param paddr physical address
    @return Pointer in kernel space
*/
static inline void* paddr2ptr(paddr_t paddr)
{
  if (is_kernel_vaddr(paddr))
    return (void*)paddr;
  if (is_kernel_paddr(paddr))
    return (void*)(paddr + kernel_p2v_offset);
  return NULL;
}

/**
    @brief Pointer convert to physical address
    @param ptr Pointer in kernel space
    @return Physical address
*/
static inline paddr_t ptr2paddr(const void* ptr) {
  if (is_kernel_vaddr((vaddr_t)ptr))
    return (paddr_t)ptr - kernel_p2v_offset;
  return NULL_PADDR;
}
#define kptr2paddr(ptr) ptr2paddr(ptr)

/**
    @brief Physical address convert to virtual address in kernel space 
    @param paddr physical address
    @return virtual address in kernel space
*/
static inline vaddr_t paddr2kaddr(paddr_t paddr) {
  if (is_kernel_vaddr(paddr))
    return NULL_VADDR;
  if (is_kernel_paddr(paddr))
    return (vaddr_t)(paddr + kernel_p2v_offset);
  return NULL_VADDR;
}

/**
    @brief Pointer of kernel convert to physical address
    @param ptr Pointer in kernel space
    @return Physical address
*/
#define kptr2paddr(ptr) ptr2paddr(ptr)

/**
    @brief Arch system page info initialize
    @param pp_syspage pointer of system page info pointer
    @return None
*/
struct syspage;
extern void arch_syspage_init(struct syspage** pp_syspage);

/**
    @brief Arch memory manager initialize
    @param p_syspage system page info pointer
    @return None
*/
extern void arch_mm_init(struct syspage* p_syspage);

/**
    @brief Arch interrupt setting initialize
    @param none
    @return None
*/
void arch_intr_init();

/**
    @brief Arch mmu struct initialize
    @param mmu Arch mmu struct pointer
    @param pde physical address of PML4 page
    @return None
*/
extern void arch_pte_init(struct arch_mmu* mmu, paddr_t pde);

/**
    @brief Arch mmu free all PTE pages
    @param mmu Arch mmu struct pointer
    @return None
*/
extern void arch_pte_free(struct arch_mmu* mmu);

/**
    @brief Get PML4 page pointer in arch mmu struct
    @param mmu Arch mmu struct pointer
    @return Pointer of PML4
*/
static inline paddr_t* arch_pte_entry_ptr(const struct arch_mmu* mmu) {
  return mmu->pde;
}

/**
    @brief Get PML4 physical address in arch mmu struct
    @param mmu Arch mmu struct pointer
    @return Physical address of PML4
*/
static inline paddr_t arch_pte_get_entry(const struct arch_mmu* mmu) {
  return ptr2paddr(arch_pte_entry_ptr(mmu));
}

/**
    @brief arch mmu pte lock
    @param mmu Arch mmu struct pointer
    @return None
*/
static inline void arch_pte_lock(struct arch_mmu* mmu) { lock_acq(&mmu->lock); }

/**
    @brief arch mmu pte unlock
    @param mmu Arch mmu struct pointer
    @return None
*/
static inline void arch_pte_unlock(struct arch_mmu* mmu) {
  lock_rel(&mmu->lock);
}

/**
    @brief MMU map virtual address to physical address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @param paddr input physical address
    @param flags memory map flags
    @return error code, 'EOK' is successful
*/
extern error_t arch_pte_map(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t paddr,
                            uint32_t flags);

/**
    @brief MMU map virtual address to physical address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @param paddr input physical address
    @param size map size
    @param flags memory map flags
    @return error code, 'EOK' is successful
*/
extern error_t arch_map_area(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t paddr,
                     size_t size, uint32_t flags);

/**
    @brief MMU unmap from virtual address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
     @param p_paddr physical address pointer for output
   @return error code, 'EOK' is successful
*/
extern error_t arch_pte_unmap(struct arch_mmu* mmu, vaddr_t vaddr,
                              paddr_t* p_paddr);

/**
    @brief MMU unmap from virtual address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @param size unmap size
    @return error code, 'EOK' is successful
*/
extern error_t arch_unmap_area(struct arch_mmu* mmu, vaddr_t vaddr, size_t size);

/**
    @brief Get physical address by virtual address
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @return physical address, NULL_PADDR for failed
*/
extern paddr_t arch_pte_get(struct arch_mmu* mmu, vaddr_t vaddr);

/**
    @brief Set pte attr to readonly
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @return error code, 'EOK' is successful
*/
extern error_t arch_pte_set_ro(struct arch_mmu* mmu, vaddr_t vaddr);

/**
    @brief Set pte attr to read & write
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @return error code, 'EOK' is successful
*/
extern error_t arch_pte_set_rw(struct arch_mmu* mmu, vaddr_t vaddr);

/**
    @brief Set pte attr to user access
    @param mmu Arch mmu struct pointer
    @param vaddr virtual address
    @return error code, 'EOK' is successful
*/
extern error_t arch_pte_set_user(struct arch_mmu* mmu, vaddr_t vaddr);

/**
    @brief Copy page-dir from kernel pte
    @param mmu Arch mmu struct pointer of user
    @param ker_mmu Arch mmu struct pointer of kernel
    @return None
*/
extern void arch_pte_copy_kernel(struct arch_mmu* mmu, const struct arch_mmu* ker_mmu);

#endif
