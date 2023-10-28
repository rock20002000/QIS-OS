/*
 * memperf.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

//#define	TVARS		uint64_t	start, stop, diff;int			i;
#define	START(x)	clock_gettime(0, &pstart);start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;for (i = 0; i < (x); i++)//#define	START(x)	start = ClockCycles();for (i = 0; i < (x); i++)

#define	STOP		clock_gettime(0, &pstart);stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000; diff = stop - start;	//	#define	STOP		stop = ClockCycles(); diff = stop - start;
#define	PRINT		print_times(diff, i, size);

#define	ITER_CPY		10000000
#define	MAX_MSGSIZE		64*1024

char	*cmd;

void
print_times(uint64_t time, int iter, int size)
{
	uint64_t	cps = 100000;	//	SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	double		tot;
	char szTmp[1024] = {0};
	tot = (double)time * 1000000.0 / (double)cps / (double)iter;

	sprintf(szTmp, " %d", (int)(size * 1e6 / tot / 1024.0 / 1024.0) );
	printf_s(szTmp);
	//	fflush(stdout);
}


void
readword(void *p, int size)
{
	volatile uint32_t	*u = p;
	uint32_t	dummy;
	int			i;

	for (i = 0; i < size / sizeof(dummy); i++)
		dummy = *u++;
}



int
main(int argc, char *argv[])
{
	int		size;
	void	*csrc;
	void	*cdst;
	void	*umem;
	uint64_t	start, stop, diff;int			i;	//	TVARS
	char szTmp[1024] = {0};

	// for automation
	//teststart(argv[0]);
	//testpntbegin(argv[0]);

	if ((csrc = mmap(0, MAX_MSGSIZE, PROT_READ, MAP_ANON | MAP_PRIVATE, NOFD, 0)) == (void *)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	if ((cdst = mmap(0, MAX_MSGSIZE, PROT_READ|PROT_WRITE, MAP_ANON | MAP_PRIVATE, NOFD, 0)) == (void *)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	if ((umem = mmap(0, MAX_MSGSIZE, PROT_READ|PROT_WRITE|PROT_NOCACHE, MAP_ANON  | MAP_PRIVATE, NOFD, 0)) == (void *)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	sprintf(szTmp, "csrc=%x cdst=%x umem=%x\n", csrc, cdst, umem);
	printf_s(szTmp);
	printf_s("Speed is MB/s\n");
	printf_s(" size cpy C->C cpy U->C cpy C->U cpy U->U    set C    set U   read C   read U\n");
	printf_s(" ---- -------- -------- -------- --------    -----    -----   ------   ------\n");
	//	fflush(stdout);
	for (size = 16; size <= MAX_MSGSIZE; size <<= 1) {
		sprintf(szTmp, "%d", size); printf_s(szTmp);	// printf_s("%5d", size);	//	fflush(stdout);

		//printf_s("csrc\n");
		// cached -> cached
		START(ITER_CPY/size)
			memcpy(cdst, csrc, size);
		STOP
		PRINT
		//printf_s("umem\n");
		// uncached -> cached
		START(ITER_CPY/size)
			memcpy(cdst, umem, size);
		STOP
		PRINT
		//printf_s("umem only\n");
		// cached -> uncached
		START(ITER_CPY/size)
			memcpy(umem, umem, size);
		STOP
		PRINT
		//printf_s("cdst umem\n");
		// uncached -> uncached
		START(ITER_CPY/size)
			memcpy(cdst, umem, size);
		STOP
		PRINT

		//printf_s("memset cdst\n");
		// cached
		START(ITER_CPY/size)
			memset(cdst, 0, size);
		STOP
		PRINT

		//printf_s("memset umem\n");
		// uncached
		START(ITER_CPY/size)
			memset(umem, 0, size);
		STOP
		PRINT

		//printf_s("read csrc\n");
		// cached
		START(ITER_CPY/size)
			readword(csrc, size);
		STOP
		PRINT

		//printf_s("read umem\n");
		// uncached
		START(ITER_CPY/size)
			readword(umem, size);
		STOP
		PRINT

		printf_s("\n");
	}

	//fflush(stdout);

    // for automation
    //testpntpass("");
    //testpntend();
    //teststop(argv[0]);

	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/

