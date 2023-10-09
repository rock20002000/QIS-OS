/*
 * timer.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


/*
#define FUTURE	3600
#define LIMIT_DIV 100

jmp_buf		timeout;

void tooslow(int signo)
{
	signal(signo, SIG_IGN);
	longjmp(timeout, (int)"timer test too slow");
}

int main(int argc, char *argv[])
{
struct sigevent		event, *sigevent;
struct itimerspec	* volatile remaining;
struct itimerspec	to, left;
clock_t				start, stop;
timer_t				timer, t, *timers;
int					volatile limit, loop, install, opt, result, destroy;
int					volatile absolute, multiple;
char				pass[80], *unres;

	teststart(argv[0]);
	testpntbegin(argv[0]);

#if defined(__QNXNTO__)
	SIGEV_SIGNAL_INIT(&event, SIGALRM);
#else
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGALRM;
#endif
	sigevent = NULL;
	install = 0, remaining = NULL, absolute = 0, multiple = 0;
	while ((opt = getopt(argc, argv, "aemotd")) != -1) {
		if (opt == 'a')
			absolute = TIMER_ABSTIME;
		else if (opt == 'e')
			sigevent = &event;
		else if (opt == 'm')
			multiple += 500;
		else if (opt == 'o')
			remaining = &left;
		else if (opt == 't')
		{
			install = !0;
			destroy = 0;
		}
		else if (opt == 'd')
		{
			install = !0;
			destroy = !0;
		}
	}
	limit = calibrate_loop();

	if (install) {
		limit /= LIMIT_DIV;
		timers = calloc( limit, sizeof( timer_t ) );
		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			timer_create(CLOCK_REALTIME, sigevent, &timers[loop]);
		}
		stop = myclock();
		if ( !destroy )
		{
			sprintf(pass, "average %stimer create = %3.3f usec", (sigevent != NULL) ? "event " : "", 1000.0 * (double)(stop - start) / (double)limit);
			testpntpass(pass);
		}

		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			timer_delete(timers[loop]);
		}
		stop = myclock();
		if ( destroy )
		{
			sprintf(pass, "average %stimer destroy = %3.3f usec", (sigevent != NULL) ? "event " : "", 1000.0 * (double)(stop - start) / (double)limit);
			testpntpass(pass);
		}

		free( timers );
	}
	else {
		if ((unres = (char *)setjmp(timeout)) != NULL) {
			testpntunres(unres);
		}
		else {
			signal(SIGALRM, tooslow);
			if (multiple != 0) {
				limit /= 2;
				to.it_value.tv_sec = (absolute ? time(NULL) + FUTURE : FUTURE) - 1, to.it_value.tv_nsec = 0;
				to.it_interval.tv_sec = to.it_interval.tv_nsec = 0;
				for (loop = 0; loop < multiple; ++loop) {
					if (timer_create(CLOCK_REALTIME, &event, &t) != EOK)
						longjmp(timeout, (int)"multiple timers exhausted");
					timer_settime(t, absolute, &to, NULL);
				}
			}
			result = timer_create(CLOCK_REALTIME, &event, &timer);
			assert(result == EOK);
			to.it_value.tv_sec = absolute ? time(NULL) + FUTURE : FUTURE, to.it_value.tv_nsec = 0;
			to.it_interval.tv_sec = to.it_interval.tv_nsec = 0;
			start = myclock();
			for (loop = limit; loop != 0; --loop) {
				timer_settime(timer, absolute, &to, remaining);
			}
			stop = myclock();
			timer_delete(timer);
			signal(SIGALRM, SIG_IGN);
			sprintf(pass, "average %s%s timer arm%s = %3.3f usec", (multiple != 0) ? "(multiple) " : "", absolute ? "absolute" : "relative", (remaining != NULL) ? " with remainder" : "", 1000.0 * (double)(stop - start) / (double)limit);
			testpntpass(pass);
		}
	}

	testpntend();
	teststop(argv[0]);
	return(EXIT_SUCCESS);
}

*/

#include <platform/include/platform.h>
//#include <libc/include/dispatch.h>
//#include <libc/include/iofunc.h>
#include <libc/include/iomsg.h>
#include <platform/include/resmgr.h>


#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

#define _PULSE_CODE_MINAVAIL	0
#define _PULSE_CODE_MAXAVAIL	127

typedef union {
        struct _pulse   pulse;
        /* your other message structures would go here too */
} my_message_t;
 
int main(int argc, char *argv[])
{
   struct sigevent         event;
   struct itimerspec       itime;
   timer_t                 timer_id;
   int                     chid;
   int                     rcvid;
   my_message_t            msg;
   struct _pulse   pulse;
 
   /* sigevent set up */
   chid = ChannelCreate(0);
 
   event.notify = SIGEV_PULSE;
   pid_t pid = getpid();
 
   event.coid = ConnectAttach(0, pid, chid, 0, 0); // --> chid
   //event.sigev_priority = getprio(0);
   //event.priority = SIGEV_PULSE_PRIO_INHERIT;
   event.priority = 10;
 
   event.code = _PULSE_CODE_MAXAVAIL; //_PULSE_CODE_MINAVAIL --> msg.pulse.code
   event.value = 123;
 
   /* timer create */
   //TimerCreate(CLOCK_REALTIME, &event);
   timer_create(CLOCK_REALTIME, &event, &timer_id);
 #if 1
   // timer expire value
   //itime.it_value.tv_sec = 0; //启动时间
   itime.it_value.tv_sec = 0; //启动时间
   /* 500 million nsecs = .5 secs */
   itime.it_value.tv_nsec = 1000000; //时间偏移.5
   //itime.it_value.tv_nsec = 10000000;
 
   // timer reload value - restart/repeat
   itime.it_interval.tv_sec = 0;  //间隔时间,间隔1S触发
   /* 500 million nsecs = .5 secs */
   itime.it_interval.tv_nsec = 1000000; //时间偏移.5
   //itime.it_interval.tv_nsec = 0; //时间偏移.5
	//timer_settime(timer_id, 0, &itime, NULL);
   TimerSettime(timer_id, 0, &itime, NULL);
 #endif
	// itime.it_value.tv_sec = 0;
    // itime.it_value.tv_nsec = 1000000;
    // itime.it_interval.tv_sec = 0;
    // itime.it_interval.tv_nsec = 0;
	// TimerSettime(timer_id, 0, &itime, NULL);

   /*
    * As of the timer_settime(), we will receive our pulse
    * in 1.5 seconds (the itime.it_value) and every 1.5
    * seconds thereafter (the itime.it_interval)
    */
	int i = 0;
   for (i = 0; i < 10 ; i++) 
   {
       //rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
	   rcvid = MsgReceivePulse(chid, &pulse, sizeof(pulse), NULL);
	   xprintf("rcvid = %d, current loop number is %d\r\n",rcvid,i);
	   xprintf("msg.pulse.code is %d\r\n",pulse.code); 
	   xprintf("msg.pulse.value is %d\r\n",pulse.value); 
	    if (pulse.code == _PULSE_CODE_MAXAVAIL) 
		{
			xprintf("timer trigger\r\n");  				
		} /* else other pulses ... */   
   }
   TimerDestroy(timer_id);
}
