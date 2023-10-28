/** \file mm.h
 *
 * Copyright 2022, QIS 0.11 Software Systems. All Rights Reserved.
 *
 * @author   	WangYang
 * @email   	yang.wang@kunyuos.com
 * @version	V1.0
 * @date    	2022-08-31
 * @brief   	Memory manager define & interface
 *
 * Version|Auther|Date|Describe
 * ------|----|------|--------
 * V1.0|WangYang|2022-08-31|Create File
 * V1.1|WangYang|2022-10-18|Sync map flags with POSIX
 */
#ifndef __MM_H__
#define __MM_H__
#include <boot/include/bootinfo.h>
#include <platform/include/mmap.h>
#include <utils/include/list.h>
#include <vdso.h>

#include "pte.h"

/**
 * ponier of vdso shared data
 */
extern vdso_data_t* p_vdso_data;

#define MM_DEBUG_BIT(d, b) ((d) << b)
#define MM_DEBUG_BUDDY(i) MM_DEBUG_BIT(0x00000001, i)
#define MM_DEBUG_SLOB(i) MM_DEBUG_BIT(0x00000100, i)
#define MM_DEBUG_VMM(i) MM_DEBUG_BIT(0x00001000, i)
extern uint32_t g_mmdebug;

/**
 * The max order for alloc pages
 */
#define PAGE_MAX_ORDER (15)

/**
 * Page attributes of memory zone
 */
typedef enum {
  KMEM,  // for kernel memory
  PMEM,  // for physical memory
  VMEM,  // for virtual memory of user
  MEMS
} mem_type_t;
#define MEM_TYPE_MASK 0x7F
#define MEM_FLAGS_ZERO 0x80
#define MEM_TYPE_FLAGS(t) (1 << (t))
#define MEM_FLAGS_TYPE(f)   ((f)) & MEM_TYPE_MASK)

/**
 * Memory map flags
 */
#define MAP_CONTIG          MAP_SPARE1  // map a continual physical space
#define MAP_VMEM            MAP_SPARE2    // map a virtual space with process ID
#define MAP_PTE             MAP_SPARE3
#define MAP_KERNEL          MAP_SPARE4

#define MAP_FLAGS_RO        (PROT_READ | MAP_PRIVATE)
#define MAP_FLAGS_CODE      (PROT_EXEC | MAP_FLAGS_RO)
#define MAP_FLAGS_DATA      (PROT_READ_WRITE | MAP_FLAGS_RO)
#define MAP_FLAGS_BSS       (MAP_FLAGS_DATA | MAP_LAZY | MAP_ANON)
#define MAP_FLAGS_STACK     (MAP_FLAGS_DATA | MAP_LAZY | MAP_STACK)
#define MAP_FLAGS_IO        (PROT_READ | MAP_PRIVATE | PROT_NOCACHE)

#define KERNEL_MAP_RFLAGS   (PROT_READ | MAP_SHARED | MAP_KERNEL)
#define KERNEL_MAP_WFLAGS   (PROT_WRITE | MAP_SHARED | MAP_KERNEL)
#define KERNEL_MAP_XFLAGS   (PROT_READ | PROT_EXEC | MAP_SHARED | MAP_KERNEL)
#define KERNEL_MAP_FLAGS    (PROT_READ_WRITE | MAP_SHARED | MAP_KERNEL)
#define KERNEL_MAP_IOFLAGS  (KERNEL_MAP_WFLAGS | PROT_NOCACHE)
#define KERNEL_MAP_PTEFLAGS (MAP_PTE | PROT_READ_WRITE | MAP_ANON | MAP_PRIVATE)


#define IS_MAP_FLAGS(f, m) (((f) & (m)) == (m))

/**
    @brief map flags to page flags
    @param flags memory map attribute
    @return page attribute
*/
static inline uint32_t map_flags_to_mem_flags(uint32_t map_flags) {
  uint32_t mem_flags = VMEM;

  if (MAP_KERNEL & map_flags)
    mem_flags = KMEM;
  else if (MAP_PHYS & map_flags)
    mem_flags = PMEM;
  if (MAP_ANON & map_flags) mem_flags |= MEM_FLAGS_ZERO;
  return mem_flags;
}

enum { VMM_NULL, VMM_STACK, VMM_APP, VMM_LIBS, VMM_SHM };

typedef struct _mm_block {
  struct _mm_block* next;
  vaddr_t start;
  vaddr_t end;
  uint32_t flags;
  id_t id;
  union {
    offset_t off;
    paddr_t pmem;
    vaddr_t vmem;
  };
} mm_block_t;

typedef struct {
  list_elem_t list;
  mm_block_t* used;
  mm_block_t* freed;
  vaddr_t base;
  vaddr_t top;
  spinlock_t lock;
  int type;
} mm_area_t;

typedef struct mm_space {
  list_t list;
  arch_mmu_t* mmu;
  mm_block_t* last_pmb;
  size_t vmem_pages;
  size_t pmem_pages;
  uint32_t vstack_pages;
  uint32_t pstack_pages;
} mm_space_t;

typedef struct {
  vaddr_t rodata_end;  // for vdso
  vaddr_t bss_start;   // for cmdline
  vaddr_t vdso_data;
  vaddr_t vdso_text;
  vaddr_t stack_top;
  uint32_t code_pages;
  uint32_t data_pages;
} mm_address_t;

struct PROCESS;
typedef struct PROCESS PROCESS_t;

// buddy.c
/**
    @brief Memory manager initialize
    @param p_syspage system page info pointer
    @return None
*/
extern void mem_init(syspage_t* p_syspage);

/**
    @brief Print memory manager infomation
    @return None
*/
extern void mem_info(void);

/**
    @brief Get memory infomation
    @param p_meminfo meminfo pointer for output
    @return None
*/
extern void get_meminfo(mem_info_t* p_meminfo);

/**
    @brief Check the physical address is a valid rom memory address
    @param paddr Physical memory address
    @return True for valid rom address
*/
extern bool is_rom_paddr(paddr_t paddr);

/**
    @brief Check the physical address is a ram memory address
    @param paddr Physical memory address
    @return True for ram memory address
*/
extern bool is_ram_paddr(paddr_t paddr);

/**
    @brief Check the physical address is a usable ram memory address
    @param paddr Physical memory address
    @return True for usable ram memory address
*/
extern bool is_usable_paddr(paddr_t paddr);

/**
    @brief Check the io address is a valid io address
    @param paddr Physical io address
    @return True for valid pointer
*/
extern bool is_io_paddr(paddr_t paddr);

/**
    @brief Check the pointer is a valid pointer in kernel space
    @param ptr pointer in kernel space
    @return True for valid pointer
*/
#define is_usable_ptr(ptr) is_usable_paddr(ptr2paddr(ptr))

/**
    @brief Get max physical pages
    @return Max physical address
*/
extern paddr_t mem_max_paddr(void);

/**
    @brief Alloc continual physical pages
    @param count pages count for alloc
    @param flags page attribute
    @return Physical address
*/
extern paddr_t alloc_pages(uint32_t count, uint32_t flags);

/**
    @brief Add reference of continual pages by physical address
    @param paddr Physical address
    @return prev reference count
*/
extern uint32_t reference_pages(paddr_t paddr);

/**
    @brief Free continual pages by physical address
    @param paddr Physical address
    @return error code, 'EOK' is successful
*/
extern error_t free_pages(paddr_t paddr);

// slob.c
/**
    @brief Malloc kernel memory
    @param size malloc bytes
    @param flags page attribute
    @return Pointer in kernel space
*/
extern void* kmalloc(size_t size, uint32_t flags);

/**
    @brief Malloc kernel memory and clear to zero
    @param size malloc bytes
    @return Pointer in kernel space
*/
static inline void* kmallocz(size_t size) {
  return kmalloc(size, MEM_FLAGS_ZERO | KMEM);
}

/**
    @brief Free kernel memory by a pointer
    @param ptr pointer in kernel space
    @return None
*/
extern void kfree(void* ptr);

/**
    @brief Get the malloc space size by a pointer
    @param ptr pointer in kernel space
    @return Malloc kernel memory bytes at pointer
*/
extern size_t mem_size(const void* ptr);

/**
    @brief Realloc kernel memory by a old pointer and new size
    @param ptr pointer alloced
    @param new_size new size for realloc
    @return New pointer alloced

    @note deprecated interface should not use in kernel space
*/
extern void* realloc(void* ptr, size_t new_size);

// vmm.c
/**
    @brief Initialize the virtual memory manager
    @param ker_proc kernel process pointer
    @param p_syspage system page info pointer
    @return None
*/
extern void vmm_init(PROCESS_t* ker_proc, syspage_t* p_syspage);

/**
    @brief Initialize the virtual memory space of process
    @param proc process pointer
    @return None
*/
extern void vm_init(PROCESS_t* proc);

/**
    @brief Memory map a physical space to virtual space
    @param proc process pointer for the mapping
    @param p_vaddr virtual address pointer, for input & output
    @param paddr physical address for mapping
    @param p_size map bytes pointer, for input & output
    @param flags memory map attribute
    @param id mapping id handle
    @param off offset in mapping file or virtual address in process
    @return error code, 'EOK' is successful
*/
extern error_t vm_map(PROCESS_t* proc, vaddr_t* p_vaddr, paddr_t paddr,
                      size_t* p_size, uint32_t flags, id_t id, offset_t off);

/**
    @brief Memory unmap the physical space from virtual space
    @param proc process pointer for the unmapping
    @param vaddr virtual address
    @param size unmap bytes
    @return error code, 'EOK' is successful
*/
extern error_t vm_unmap(PROCESS_t* proc, vaddr_t vaddr, size_t size);

/**
    @brief Get physical address from virtual address
    @param proc process pointer for the unmapping
    @param vaddr virtual address
    @param size area bytes
    @param shmb shm_block pointer for output
    @return error code, 'EOK' is successful
*/
extern error_t vm_offset(PROCESS_t* proc, vaddr_t vaddr, size_t size,
                         shm_block_t* shmb);

/**
    @brief Free all mmu memory in the process
    @param proc process pointer
    @return None
*/
extern void vm_free(PROCESS_t* proc);

/**
    @brief malloc a physical space and map to virtual space
    @param proc process pointer for the mapping
    @param vaddr input virtual address
    @param p_size malloc bytes pointer, for input & output
    @param flags memory map attribute
    @return virtual address, 'NULL_VADDR' is failed
*/
static inline vaddr_t vmalloc(PROCESS_t* proc, vaddr_t vaddr, size_t* p_size,
                              uint32_t flags) {
  if (proc && p_size && *p_size) {
    if (EOK == vm_map(proc, &vaddr, NULL_PADDR, p_size, flags, NOFD, 0))
      return vaddr;
  }

  return NULL_VADDR;
}

/**
    @brief Memory map a shared virtual space of other process
    @param proc process pointer for the mapping
    @param p_vaddr virtual address pointer, for input & output
    @param size shared bytes
    @param flags memory map attribute
    @param shm_pid process id for shared
    @param shm_vaddr shared virtual address in the process with shm_pid
    @return error code, 'EOK' is successful
*/
static inline error_t vm_share(PROCESS_t* proc, vaddr_t* p_vaddr, size_t size,
                               uint32_t flags, pid_t shm_pid,
                               vaddr_t shm_vaddr) {
  return vm_map(proc, p_vaddr, NULL_PADDR, &size, flags | MAP_VMEM, shm_pid,
                (offset_t)shm_vaddr);
}

/**
    @brief free the physical space by virtual space
    @param proc process pointer for the mapping
    @param vaddr virtual address
    @param size free bytes
    @param flags memory map attribute
    @return error code, 'EOK' is successful
*/
#define vfree(proc, vaddr, size) vm_unmap(proc, vaddr, size)

/**
    @brief Map a virtual address of the process to kernel space
    @param proc process pointer for the mapping
    @param user_data pointer for the mapping in user space
    @param size map size
    @return Pointer of map data in kernel space
    @note if in current process, not map, but return user pointer 
*/
extern void* kernel_map_user(PROCESS_t* proc, const void* user_data,
                             size_t size, uint32_t flags);
#ifdef USER_INCLUDE_KERNEL_PTE
/**
    @brief Map a virtual address of the process to kernel space
    @param proc process pointer for the mapping
    @param user_data pointer for the mapping in user space
    @param size map size
    @return Pointer of map data in kernel space
*/
 extern void* map_from_user(PROCESS_t* proc,
                const void* user_data, size_t size, uint32_t flags);
#else
 #define map_from_user  kernel_map_user
#endif

/**
    @brief unmap kernel space
    @param ptr unmap pointer in kernel space
    @param size unmap size
    @return error code, 'EOK' is successful
*/
extern error_t kernel_unmap(const void* ptr, size_t size);

/**
    @brief Get the physical address from user virtual address
    @param proc process pointer for the unmapping
    @param vaddr virtual address
    @return Physical address
*/
extern paddr_t user_get_paddr(PROCESS_t* proc, vaddr_t vaddr);

/**
    @brief Copy data from user space to kernel space
    @param proc process pointer copy from
    @param user_data user virtual address of source data
    @param size copy bytes
    @param kernel_data kernel address copy to
    @return error code, 'EOK' is successful
*/
extern error_t copy_from_user(PROCESS_t* proc, const void* user_data,
                              size_t size, void* kernel_data);

/**
    @brief Copy data from user space to new kernel space
    @param proc process pointer copy from
    @param user_data user virtual address of source data
    @param size copy bytes
    @return Pointer of copy data in kernel space
*/
extern void* copy_user_data(PROCESS_t* proc, const void* user_data,
                            size_t size);

/**
    @brief Copy data from kernel space to user space
    @param proc process pointer to copy
    @param user_data user virtual address to copy
    @param size copy bytes
    @param kernel_data kernel address of source data
    @return error code, 'EOK' is successful
*/
extern error_t copy_kernel_data(PROCESS_t* proc, void* user_data, size_t size,
                                const void* kernel_data);

/**
    @brief Copy data from user space to user space
    @param dst_proc process pointer copy to
    @param dst_data user virtual address to copy
    @param size copy bytes
    @param src_proc process pointer copy from
    @param src_data user virtual address of source data
    @return error code, 'EOK' is successful
*/
extern error_t user_copy_user(PROCESS_t* dst_proc, void* dst_data, size_t size,
                              PROCESS_t* src_proc, const void* src_data);

#ifdef ARCH_MAP_IO
typedef struct {
  paddr_t base;
  paddr_t top;
} arch_mem_space_t;

/**
    @brief Get map IO space info
    @param space io space pointer
    @param size space max size
    @return total io space
*/
extern int arch_get_io_space(arch_mem_space_t* space, int size);

/**
    @brief Get memory zone space info
    @param space zone space pointer
    @param size space max size
    @return total zone space
*/
extern int arch_get_mem_zone(arch_mem_space_t* space, int size);
#endif

#endif  // __MM_H__
