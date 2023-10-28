#ifndef _ARM_LTIMER_H_
#define _ARM_LTIMER_H_
#include <arch.h>
#include <interrupt.h>
#include <utils/include/list.h>

#include "ker_time.h"

// unsigned long long getTicks(void);

#define HTIMER0_IRQ 0x2
#define HPET_TIMER_VECTOR (VECTOR_IRQ_BASE + HTIMER0_IRQ)

// void  init_time(void);
extern int initTimer(Timer *timer, TimerCallBackFunction callback, int timeout,
                     void *data, size_t size, int fUserMode);

#endif
