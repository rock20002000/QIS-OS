/*
 * time.h
 *
 *  Created on: 2022-9-5
 *      Author: Test
 */

#ifndef KERNEL_KERNEL_INCLUDE_KER_TIME_H_
#define KERNEL_KERNEL_INCLUDE_KER_TIME_H_

#include <platform/include/time.h>
#include "interrupt.h"
#include <utils/include/list.h>
#include "object.h"

size_t get_systime(systime_t* p_systime);
size_t add_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_sum);
size_t sub_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_diff);
size_t systime_to_us(const systime_t* p_systime);
size_t systime_to_timespec(const systime_t* p_systime, struct timespec* ts);
size_t get_sys_timespec(struct timespec* ts);

// size_t   getTicks(void);
int 		initTimeList(void);
int 		timeInterrupt(iframe_t* reg,void *data);
typedef int (*TimerCallBackFunction)(void *arg);

typedef struct {
   struct phy_object parent;                            /**< inherit from rt_object */

   int	iTimerID;
   TimerCallBackFunction callback;
   void *data;
   int	size;
   int  timeout;
   int 	onStack;
   int	fValid;
   unsigned long int ticks;
   list_t list;
} Timer;

Timer*   createTimer(TimerCallBackFunction callback, int timeout, void *data, size_t size);
int 		addTimer(Timer *timer);

void 		get_biostime(struct tm*   pCurBiosTime);
int 		ker_clock_gettime( clockid_t clock_id, struct timespec * tp, struct tm* pInitBiosTime );
int 		ker_vdso_gettime( clockid_t clock_id, struct timespec * tp );


#endif /* KERNEL_KERNEL_INCLUDE_KER_TIME_H_ */
