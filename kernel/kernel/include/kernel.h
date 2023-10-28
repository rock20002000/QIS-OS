#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <boot/include/bootinfo.h>

#include "arch.h"
#include "ker_time.h"
#include "locks.h"
#include "mm.h"
#include "task.h"
#include "pathmgr.h"

#define ARCH_CPUVAR (&get_cpuvar()->arch)

#define CURPROCESS  (get_cpuvar()->current_task->process)

// kmain.c
extern syspage_t* g_syspage_ptr;
extern const char* g_version;

extern size_t getTicks(void);

#define KERNEL_VERSION  ((MAJOR_VERSION << 16) | (MINOR_VERSION << 8) | BUILD_NO)

static inline paddr_t system_pte(int cpu)
{
    if (cpu < 0)
        cpu = mp_self();
    return (paddr_t)g_syspage_ptr->pgdir[cpu];
}

/**
    @brief Backup pte entry and switch to kernel space
    @return current pte entry physical address
*/
static inline paddr_t arch_pte_backup(void)
{
  paddr_t pte = arch_pte_read();
#ifndef USER_INCLUDE_KERNEL_PTE
extern syspage_t* g_syspage_ptr;
  int cpu = mp_self();

  arch_pte_write((paddr_t)g_syspage_ptr->pgdir[cpu]);
#endif
  return pte;
}

/**
    @brief restore pte entry
    @param paddr pte entry physical address
    @return None
*/
static inline void arch_pte_restore(paddr_t paddr)
{
  if (NULL_PADDR != paddr)
  {
    arch_pte_write(paddr);
  }
}

#endif
