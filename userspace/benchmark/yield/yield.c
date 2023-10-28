/*
 * yield.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


/*

enum { YIELD_SELF, YIELD_THREAD, YIELD_PROCESS };

int		limit, affinity;

void *yielder(void *param)
{
int		loop;

	SYNC_PRIORITY(0, NULL);
	SYNC_COORDINATE(0, affinity);
	for (loop = limit; loop != 0; --loop) {
		YIELD();
	}
	return(NULL);
}

int main(int argc, char *argv[])
{
clock_t				start, stop;
pthread_t			thread;
pthread_attr_t		attr;
struct sched_param	param;
pid_t				child;
int					loop, mode, pri, alg, opt, result;
char				pass[80];

	teststart(argv[0]);
	testpntbegin(argv[0]);

	mode = YIELD_PROCESS, affinity = 0;
	pri = sched_get_priority_max(alg = SCHED_FIFO);
	while ((opt = getopt(argc, argv, "1aAP:t")) != -1) {
		if (opt == '1')
			mode = YIELD_SELF;
		else if (opt == 'a' && SYNC_NCPU() > 1)
			affinity = +1;
		else if (opt == 'A' && SYNC_NCPU() > 1)
			affinity = -1;
		else if (opt == 'P')
			pri = atoi(optarg);
		else if (opt == 't')
			mode = YIELD_THREAD;
	}
	limit = calibrate_loop();

	SYNC_INIT("yield", mode == YIELD_SELF);
	sched_setscheduler(0, alg, (param.sched_priority = pri, &param));
	switch (mode) {
	case YIELD_SELF:
		SYNC_COORDINATE(!0, affinity);
		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			YIELD();
		}
		stop = myclock();
		break;
	case YIELD_THREAD:
		pthread_attr_init(&attr);
		SYNC_PRIORITY(0, &attr);
		result = pthread_create(&thread, &attr, yielder, NULL);
		assert(result == EOK);
		SYNC_COORDINATE(!0, affinity);
		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			YIELD();
		}
		stop = myclock();
		pthread_join(thread, NULL);
		pthread_attr_destroy(&attr);
		limit *= 2;
		break;
	case YIELD_PROCESS:
		if (!(child = fork())) {
			SYNC_PRIORITY(0, NULL);
			SYNC_COORDINATE(0, affinity);
			for (loop = limit; loop != 0; --loop) {
				YIELD();
			}
			_exit(0);
		}
		assert(child != -1);
		SYNC_PRIORITY(0, NULL);
		SYNC_COORDINATE(!0, affinity);
		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			YIELD();
		}
		stop = myclock();
		waitpid(child, NULL, 0);
		limit *= 2;
		break;
	default:
		start = stop = myclock();
		break;
	}
	SYNC_DESTROY();

	sprintf(pass, "average %s%s yield = %3.3f usec", (mode == YIELD_SELF) ? "self" : (mode == YIELD_THREAD) ? "inter-thread" : "inter-process", (affinity > 0) ? " same-CPU" : (affinity < 0) ? " diff-CPU" : "", 1000.0 * (double)(stop - start) / (double)limit);
	testpntpass(pass);
	testpntend();
	teststop(argv[0]);
	return(EXIT_SUCCESS);
}

*/


#include <platform/include/platform.h>

enum { YIELD_SELF, YIELD_THREAD,YIELD_LOW_PRIO,YIELD_HIGH_PRIO,YIELD_MUILTI_THREAD, YIELD_PROCESS };

int		limit,affinity;

void *yielder(void *param)
{
	int	loop;
    xprintf("thread yielder is running\r\n");
	for (loop = limit; loop != 0; --loop)
	{
		//printf_s("yielder:before yield\n");
		int res = yield();
		//xprintf("child thread called yield,it's return value is res = %d,loop number is %d\r\n",res,loop);
		//printf_s("yielder:after yield\n");
	}
	xprintf("thread yielder is going to exit\r\n");
	return(NULL);
}

//child thread with same priority
void *thread_fun1(void *param)
{
	int	loop;
    xprintf("thread_fun1 created,with the same priority.\r\n");
	for (loop = limit; loop != 0; --loop)
	{
		int res = yield();
		xprintf("thread_fun1 called yield,it's return value is res = %d,current loop number is %d\r\n",res,loop);
	}
	xprintf("thread_fun1 is going to exit.\r\n");
	return(NULL);
}
//child thread with same priority.
void *thread_fun2(void *param)
{
	int	loop;
    xprintf("thread_fun2 created,with the same priority.\r\n");
	for (loop = limit; loop != 0; --loop)
	{
		int res = yield();
		xprintf("thread_fun2 called yield,it's return value is res = %d,current loop number is %d\r\n",res,loop);
	}
	xprintf("thread_fun2 is going to exit.\r\n");
	return(NULL);
}

void *thread_fun3(void *param)
{
	int	loop;
    xprintf("thread_fun3 created,with the same priority.\r\n");
	for (loop = limit; loop != 0; --loop)
	{
		int res = yield();
		xprintf("thread_fun3 called yield,it's return value is res = %d,current loop number is %d\r\n",res,loop);
	}
	xprintf("thread_fun3 is going to exit.\r\n");
	return(NULL);
}

clock_t myclock(struct timespec* pstart,clock_t current_time)
{
	clock_gettime(0, pstart);
	current_time = (pstart->tv_sec) * 1000000 + (pstart->tv_nsec)/1000;
	return current_time;
}

int main(int argc, char *argv[])
{
	struct timespec 	pstart,pstop;
	clock_t				start, stop;
	pthread_t			thread;
	pthread_t			thread_id1;
	pthread_t			thread_id2;
	pthread_t			thread_id3;

	//pid_t				child;
	int					loop, mode, pri, alg, opt, result;
	char				pass[80];
	double  spendtime = 0.0;

	mode = YIELD_THREAD;
	affinity = 1;
	limit = 1000;
	if (argc <= 1) 
	{
		mode = YIELD_SELF;
	} else if (argc <= 2) 
	{
		mode = atoi(argv[1]);
	}

	switch (mode)
	{
		case YIELD_SELF:
		{
			clock_gettime(0, &pstart);
			start =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			for (loop = limit; loop != 0; --loop)
			{
				int res = yield();
				//xprintf("called,yield-self,its return value is res: %d\r\n",res);
			}
			clock_gettime(0, &pstart);
			stop =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			sprintf(pass, "average %s%s yield = %d usec\r\n", (mode == YIELD_SELF) ? "self" : (mode == YIELD_THREAD) ? "inter-thread" : "inter-process", (affinity > 0) ? " same-CPU" : (affinity < 0) ? " diff-CPU" : "", (stop - start)/limit );
			xprintf(pass);
			//xprintf("\r\neverage yield time is %d nsecs: \r\n",spendtime / (double)limit);
			break;
		}		
		case YIELD_THREAD:
		{
			xprintf("create a new thread yielder \r\n");
			result = pthread_create(&thread, NULL, yielder, NULL);
			xprintf("now it is in main thread.\r\n");
			clock_gettime(0, &pstart);
			start =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			for (loop = limit; loop != 0; --loop)
			{
				int res = yield();
				//xprintf("maid thread called yield,it's return value is res = %d,loop number is %d\r\n",res,loop);
				//xprintf("main:after yield\r\n");
			}
			clock_gettime(0, &pstart);
			stop =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			xprintf("main thread:11111111111111111\n");
			pthread_join(thread, NULL);
			xprintf("main thread:22222222222222222\n");
			//pthread_attr_destroy(&attr);
			limit *= 2;
			sprintf(pass, "average %s%s yield = %d usec\r\n", (mode == YIELD_SELF) ? "self" : (mode == YIELD_THREAD) ? "inter-thread" : "inter-process", (affinity > 0) ? " same-CPU" : (affinity < 0) ? " diff-CPU" : "", (stop - start)/limit);
			xprintf(pass);
			//xprintf("\r\neverage yield time is %d nsecs: \r\n",spendtime / (double)limit);
			break;
		}
		
		case YIELD_LOW_PRIO:
		{		
			break;
		}
		case YIELD_HIGH_PRIO:
		{
			break;
		}
		case YIELD_MUILTI_THREAD:
		{
			xprintf("create a new thread:thread_fun1\r\n");
			result = pthread_create(&thread_id1, NULL, thread_fun1, NULL);
			xprintf("create a new thread:thread_fun2\r\n");
			result = pthread_create(&thread_id2, NULL, thread_fun2, NULL);
			xprintf("create a new thread:thread_fun3\r\n");
			result = pthread_create(&thread_id3, NULL, thread_fun3, NULL);
			
			xprintf("mark:now, it is in main thread\r\n");
			for (loop = limit; loop != 0; --loop)
			{
				int res = yield();
				xprintf("maid thread called yield,it's return value is res = %d,loop number is %d\r\n",res,loop);
			}
			break;
		}
		case YIELD_PROCESS:
		{
			//us
			clock_gettime(0, &pstart);
			start =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			xprintf("yield main process: started!\r\n");
			setprio_critical(0,20);
			shell_exec("yieldchild");
			for (loop = limit; loop != 0; --loop)
			{
				xprintf("yield main process: run:%d!\r\n", loop);
				int res = yield();
				//xprintf("yield main process,current loop number is %d \r\n",loop);
			}
			xprintf("yield main process:out of loop!\r\n");
			clock_gettime(0, &pstart);
			stop =  pstart.tv_sec*1000000 + pstart.tv_nsec/1000;
			limit *= 2;
			sprintf(pass, "average inter-process yield = %d usec.\r\n",  (stop - start)/limit);
			xprintf(pass);
			break;
		}			
		default:
		{
			start = stop = myclock(&pstart,stop);
			break;
		}			
	}
    xprintf("-----------------main is going to exit-----------------\r\n");	
	
	return(0);
}
