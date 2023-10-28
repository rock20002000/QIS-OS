#ifndef __ARM_TRAP_H__
#define __ARM_TRAP_H__

#define INTERRUPT_HANDLER_SIZE 16
// Offsets in GS frame
#define GS_RSP0 0
#define GS_RSP3 8
#define GS_KERN_PGDIR 0x30D8
#define GS_CUR_TASK_PGDIR 0x30E0
#ifndef __ASSEMBLER__
#include "arch.h"

//
// Handlers defined in trap.S.
//
typedef uint8_t interrupt_handler_t[16];
extern interrupt_handler_t interrupt_handlers[];

extern void syscall_entry(void);
extern void userland_entry(void);
extern void switch_context(uint32_t *prev_esp, uint32_t *next_esp);

///???///STATIC_ASSERT(offsetof(struct arch_cpuvar, rsp0) == GS_RSP0);
///???///STATIC_ASSERT(offsetof(struct arch_cpuvar, rsp3) == GS_RSP3);
///???///STATIC_ASSERT(offsetof(struct cpuvar, kernel_page_dir) == GS_KERN_PGDIR);
///???///STATIC_ASSERT(offsetof(struct cpuvar, cur_task_pgdir) == GS_CUR_TASK_PGDIR);
#endif  // ifndef __ASSEMBLER__

#endif
