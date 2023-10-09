/*
 * calls.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

#define __PAGESIZE		PAGE_SIZE

void do_kernop(int limit) {
	int i = 0;

	while (i++ < limit) {
		kernel_nop();
	}
}

void handler() { }

/*	void do_sig(int limit) {
	int i = 0;
	struct sigaction sa, orig;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);

	while (i++ < limit) {
		sigaction(SIGUSR1, &sa, &orig);
	}
}	*/


void do_mmap(int limit, int size, unsigned touch, unsigned flags) {

	unsigned *ptr;
	int i = 0, j = 0, pg = __PAGESIZE;

	while (i++ < limit) {
		if((ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, flags, NOFD, 0)) == MAP_FAILED) {
			printf_s("mmap failed\n");
			exit(1);
		}
		if(touch) {
			for(j = 0; j < (size / pg); j++) {
				ptr[(j * (pg >> 2))] = 0;
			}
		}
		munmap(ptr, size);
	}
}

void do_getpid(int limit) {
	int i = 0, j;

	while (i++ < limit) {
		j = getpid();
	}
}

//void do_getppid(int limit) {
//	int i = 0, j;
//	while (i++ < limit) {
//		j = getppid();
//	}
//}

int
main(int argc, char *argv[]) {
	clock_t		start, stop;
	struct timespec pstart;
	int		limit;
	char	buf[80];

	// for automation
	//teststart(argv[0]);

	setprio(0,30);
	limit = 1000;	//	calibrate_loop();

	clock_gettime(CLOCK_REALTIME, &pstart);

	// Note: myclock() returns current time in milliseconds
	// Now do benches
	printf_s("NOP kernel call\n");	//	testpntbegin("NOP kernel call");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
	do_kernop(limit);

	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
	//testpntpass(buf);
	//testpntend();

	//testpntbegin("Install Sig. handler kernel call");
	//start = myclock();
	//do_sig(limit);
	//stop = myclock();
	//SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	//testpntpass(buf);
	//testpntend();

	printf_s("Simple proc call (getppid())\n");	//testpntbegin("Simple proc call (getppid())");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
	do_getpid(limit);	//	do_getppid(limit);
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
	//testpntpass(buf);
	//testpntend();

	limit = limit / 10;

	printf_s("MMAP call followed by munmap (MAP_ANON | MAP_PRIVATE, 4096 bytes)\n");	//testpntbegin("MMAP call followed by munmap (MAP_ANON | MAP_PRIVATE, 4096 bytes)");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
//#if defined(__LINUX__)
	do_mmap(limit, 4096, 0, MAP_PRIVATE);//MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE);
//#else
//	do_mmap(limit, 4096, 0, MAP_ANON | MAP_PRIVATE);
//#endif
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
	//testpntpass(buf);
	//testpntend();

	printf_s("MMAP call + munmap(MAP_ANON | MAP_PRIVATE, 8192 bytes)\n");	//testpntbegin("MMAP call + munmap(MAP_ANON | MAP_PRIVATE, 8192 bytes)");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
//#if defined(__LINUX__)
	do_mmap(limit, 4096, 0, MAP_PRIVATE );	//MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE);
//#else
//	do_mmap(limit, 8192, 0, MAP_ANON | MAP_PRIVATE);
//#endif
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
    //testpntpass(buf);
    //testpntend();

	printf_s("MMAP call + munmap(MAP_ANON | MAP_LAZY | MAP_PRIVATE, 4096 bytes)\n");	//testpntbegin("MMAP call + munmap(MAP_ANON | MAP_LAZY | MAP_PRIVATE, 4096 bytes)");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
//#if defined(__LINUX__)
	do_mmap(limit, 4096, 0, MAP_PRIVATE);	//	MAP_ANONYMOUS | MAP_PRIVATE);
//#else
//	do_mmap(limit, 4096, 0, MAP_ANON | MAP_LAZY | MAP_PRIVATE);
//#endif
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
    //testpntpass(buf);
    //testpntend();

	printf_s("MMAP call + faulting in + munmap(MAP_ANON | MAP_LAZY | MAP_PRIVATE, 4096 bytes)\n");	//testpntbegin("MMAP call + faulting in + munmap(MAP_ANON | MAP_LAZY | MAP_PRIVATE, 4096 bytes)");
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
//#if defined(__LINUX__)
	do_mmap(limit, 4096, 1, MAP_PRIVATE);	//	MAP_ANONYMOUS | MAP_PRIVATE);
//#else
//	do_mmap(limit, 4096, 1, MAP_ANON | MAP_LAZY | MAP_PRIVATE);
//#endif
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	sprintf(buf, "avg. call time = %d usec\n", (int)(1000.0 * (double)(stop-start)/(double)limit));	//	SPRNT(buf, sizeof(buf), "avg. call time = %f usec", (1000.0 * (double)(stop-start)/(double)limit));
	printf_s(buf);
    //testpntpass(buf);
    //testpntend();

	// for automation
	//teststop(argv[0]);
	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/

