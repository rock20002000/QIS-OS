/*
 * mutex.c
 *
 *  Created on: 2022-9-16
 *      Author: Test
 */

#include <platform/include/platform.h>

#define EOK			0

pthread_mutex_t		mutex, cmutex[4];
int					limit, contested, useinline, usekernel, affinity;

/*void screen_printf(char* pszTmp){
	while((*pszTmp)!='\0'){
		scr_write(*pszTmp);	//	x64_screen_printchar(*pszTmp);
		pszTmp++;
	}
	return;
}*/

void *contestant1(void *param)
{
	int		loop;

	//SYNC_COORDINATE(0, affinity);
	for (loop = limit; loop != 0; --loop) {
		pthread_mutex_lock(&cmutex[1]);
		//YIELD();
		pthread_mutex_unlock(&cmutex[1]);
		pthread_mutex_lock(&cmutex[0]);
		//YIELD();
		pthread_mutex_unlock(&cmutex[0]);
	}
	return(NULL);
}

void *contestant2(void *param)
{
int		loop;

	pthread_mutex_lock(&cmutex[0]);
	//SYNC_COORDINATE(0, affinity);
	for (loop = limit; loop != 0; --loop) {
		pthread_mutex_lock(&cmutex[2]);
		pthread_mutex_lock(&cmutex[1]);
		pthread_mutex_unlock(&cmutex[2]);
		pthread_mutex_unlock(&cmutex[0]);
		pthread_mutex_lock(&cmutex[3]);
		pthread_mutex_lock(&cmutex[0]);
		pthread_mutex_unlock(&cmutex[3]);
		pthread_mutex_unlock(&cmutex[1]);
	}
	pthread_mutex_unlock(&cmutex[0]);
	return(NULL);
}

struct timespec 	pstart;
clock_t myclock(){
	clock_gettime(0, &pstart);
	return pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
}

void main() //int main(int argc, char *argv[])
{
	//pthread_mutexattr_t	attr;
	pthread_t			thread;
	clock_t				start, stop, ostart, overhead;
	int					loop, opt, result, unavail;
	char				pass[128];

	//teststart(argv[0]);
	//testpntbegin(argv[0]);

	contested = useinline = usekernel = 0, unavail = 0, affinity = 0;

	limit = 100000;	//	calibrate_loop();

	//	SYNC_INIT("mutex", !contested);
	//pthread_mutexattr_init(&attr);
//#if defined(PTHREAD_PRIO_PROTECT) && defined(PTHREAD_PRIO_INHERIT)
//	pthread_mutexattr_setprotocol(&attr, usekernel ? PTHREAD_PRIO_PROTECT : PTHREAD_PRIO_INHERIT);
//	pthread_mutexattr_setprioceiling(&attr, usekernel ? getprio(0) : 0);
//#endif
	//while(1){
	if (contested) {
		limit /= 4;
		result = pthread_mutex_init(&cmutex[0], NULL);	//	&attr);
		//assert(result == EOK);
		result = pthread_mutex_init(&cmutex[1], NULL);	//	&attr);
		//assert(result == EOK);
		if (contested < 0) {
			result = pthread_mutex_init(&cmutex[2], NULL);	//	&attr);
			//assert(result == EOK);
			result = pthread_mutex_init(&cmutex[3], NULL);	//	&attr);
		}
		result = pthread_create(&thread, NULL, (contested < 0) ? contestant2 : contestant1, NULL);
		//assert(result == EOK);
	}
	else {
		result = pthread_mutex_init(&mutex, NULL);	//	&attr);
		//assert(result == EOK);
	}

	if (contested < 0) {
		for (ostart = myclock(), loop = limit / 2; loop != 0; --loop)
			pthread_mutex_lock(&cmutex[2]), pthread_mutex_unlock(&cmutex[2]);
		overhead = (myclock() - ostart) * 8;
		pthread_mutex_lock(&cmutex[2]);
		pthread_mutex_lock(&cmutex[3]);
		//SYNC_COORDINATE(!0, affinity);
		start = myclock();
		pthread_mutex_unlock(&cmutex[2]);
		for (loop = limit; loop != 0; --loop) {
			pthread_mutex_lock(&cmutex[0]);
			pthread_mutex_lock(&cmutex[2]);
			pthread_mutex_unlock(&cmutex[0]);
			pthread_mutex_unlock(&cmutex[3]);
			pthread_mutex_lock(&cmutex[1]);
			pthread_mutex_lock(&cmutex[3]);
			pthread_mutex_unlock(&cmutex[1]);
			pthread_mutex_unlock(&cmutex[2]);
		}
		stop = myclock();
		pthread_mutex_unlock(&cmutex[3]);
	}
	else if (contested > 0) {
		//for (ostart = myclock(), loop = limit / 2; loop != 0; --loop)
		//	YIELD();
		overhead = (myclock() - ostart) * 8;
		pthread_mutex_lock(&cmutex[1]);
		//SYNC_COORDINATE(!0, affinity);
		start = myclock();
		for (loop = limit; loop != 0; --loop) {
			pthread_mutex_lock(&cmutex[0]);
			pthread_mutex_unlock(&cmutex[1]);
			//YIELD();
			pthread_mutex_lock(&cmutex[1]);
			pthread_mutex_unlock(&cmutex[0]);
			//YIELD();
		}
		stop = myclock();
		pthread_mutex_unlock(&cmutex[1]);
	}
	else {
		overhead = 0;
		//SYNC_COORDINATE(!0, affinity);
		start = myclock();
		if (unavail) {
			pthread_mutex_lock(&mutex);
			for (loop = limit; loop != 0; --loop) {
				pthread_mutex_trylock(&mutex);
			}
			pthread_mutex_unlock(&mutex);
		}
		else {
			for (loop = limit; loop != 0; --loop) {
				pthread_mutex_lock(&mutex);
				pthread_mutex_unlock(&mutex);
			}
		}
		stop = myclock();
	}

	if (contested) {
		pthread_join(thread, NULL);
		if (contested < 0) {
			pthread_mutex_destroy(&cmutex[3]);
			pthread_mutex_destroy(&cmutex[2]);
		}
		pthread_mutex_destroy(&cmutex[1]);
		pthread_mutex_destroy(&cmutex[0]);
		limit *= 4;
	}
	else {
		pthread_mutex_destroy(&mutex);
	}
	//pthread_mutexattr_destroy(&attr);
	//SYNC_DESTROY();

	//xprintf("average %s %s%s mutex %s = %d usec\n", unavail ? "unavailable" : (contested < 0) ? "contested" : (contested > 0) ? "contested/yield" : "uncontested", contested ? "" : usekernel ? "kernel " : "atomic ", useinline ? "inline" : "libc", unavail ? "trylock" : "lock/unlock", (stop - start - overhead));	//	1000.0 * (double)(stop - start - overhead) / (double)limit);
	//}

	sprintf(pass, "average %s %s%s mutex %s = %d usec\n", unavail ? "unavailable" : (contested < 0) ? "contested" : (contested > 0) ? "contested/yield" : "uncontested", contested ? "" : usekernel ? "kernel " : "atomic ", useinline ? "inline" : "libc", unavail ? "trylock" : "lock/unlock", (stop - start - overhead));
	screen_printf(pass);

	//	sprintf(pass, "average %s %s%s mutex %s = %3.3f usec", unavail ? "unavailable" : (contested < 0) ? "contested" : (contested > 0) ? "contested/yield" : "uncontested", contested ? "" : usekernel ? "kernel " : "atomic ", useinline ? "inline" : "libc", unavail ? "trylock" : "lock/unlock", 1000.0 * (double)(stop - start - overhead) / (double)limit);
	//testpntpass(pass);
	//testpntend();
	//teststop(argv[0]);
	return ;	//(EXIT_SUCCESS);
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
