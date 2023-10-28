/*
 * time.h
 *
 *  Created on: 2022-9-5
 *      Author: Test
 */

#ifndef SDK_LIB_PLATFORM_INCLUDE_TIME_H_
#define SDK_LIB_PLATFORM_INCLUDE_TIME_H_

#include <types.h>
#include <object.h>

#define CLOCK_REALTIME             0
#define CLOCK_SOFTTIME             1
#define CLOCK_MONOTONIC            2
#define CLOCK_PROCESS_CPUTIME_ID   3
#define CLOCK_THREAD_CPUTIME_ID    4

struct tm {
int tm_sec;
int tm_min;
int tm_hour;			// 0~24
int tm_mday;			// 0~31
int tm_mon; 			// 1~12
int tm_year; 			// year - 2000
int tm_wday; 			// 1 ~ 7
int tm_yday; 			// 1~365
int tm_isdst; 			//whether using summer time: 1 using; 0 not; -1 unknown
long int tm_gmtoff; 	// negative seconds
const char *tm_zone; 	// time zone
};



typedef long		time_t;		//	For used by time functions, such as date() and so on

typedef int			timer_t;	//	For used by timer_createtimer and so on

typedef uint32_t	clock_t;

typedef int			clockid_t;		//	#define clockid_t		int

struct timespec {
   time_t   tv_sec;
   long     tv_nsec;
};

struct _itimer
{
    struct timespec it_value;
	struct timespec it_interval;
};

struct itimerspec {
	struct timespec it_value;
	struct timespec	it_interval;
} ;

typedef struct {
   size_t ticks;
   size_t cycles;
} systime_t;

#ifndef PHY_TIMER_SKIP_LIST_LEVEL
#define PHY_TIMER_SKIP_LIST_LEVEL  1
#endif


struct phy_timer
{
    struct phy_object parent;                            /**< inherit from rt_object */

    list_t row[PHY_TIMER_SKIP_LIST_LEVEL];

    void (*timeout_func)(void *parameter);              /**< timeout function */
    void  *parameter;                         /**< timeout function's parameter */

    phy_tick_t init_tick;                         /**< timer timeout tick */
    phy_tick_t timeout_tick;                      /**< timeout tick */
};

static inline time_t time(time_t *tloc)
{
extern int clock_gettime(clockid_t, struct timespec*);

    struct timespec ts;

    clock_gettime(0, &ts);

    time_t ret = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    if (tloc)
        *tloc = ret;
    return ret;
}
#endif /* SDK_LIB_PLATFORM_INCLUDE_TIME_H_ */
