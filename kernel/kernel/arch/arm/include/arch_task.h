#ifndef __ARM_TASK_H__
#define __ARM_TASK_H__

#include "random32.h"
#include "task.h"

#define THREAD_STACK_SIZE_KERNEL    PAGE_SIZE*4
#define THREAD_STACK_SIZE           PAGE_SIZE*8

void arch_thread_init(THREAD_t* thread, uint64_t function_addr);
void arch_thread_init_withParam(THREAD_t* thread, uint64_t function_addr,
                                void* threadParams, void* extParams,
                                THREAD_t* pCurThread);
void arch_task_switch(THREAD_t* prev, THREAD_t* next);
void arch_task_switch_intr(THREAD_t* prev, THREAD_t* next, iframe_t* iframe);
void arch_thread_destroy(THREAD_t* thread);

void arch_enable_irq(unsigned irq);
void arch_disable_irq(unsigned irq);

void gen_softintr();
#endif
