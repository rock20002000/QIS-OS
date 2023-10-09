
#ifndef __PER_CPU_H__
#define __PER_CPU_H__

#include <arch.h>
//#include "task.h"
/// CPU-local variables.

struct arch_cpuvar;
struct cpuvar {
    struct arch_cpuvar arch;
    THREAD_t *current_task;
    //	paddr_t  kernel_page_dir;
    //	paddr_t  cur_task_pgdir;

    THREAD_t idle_task;
    uint16_t cpu_id;
}/*__attribute__((packed))*/;


typedef struct cpuvar* cpuvar_ptr;

#endif
