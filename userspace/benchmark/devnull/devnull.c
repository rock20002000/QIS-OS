/*
 * devnull.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

int testloop(int fd, int size, int limit) {
	char *b;
	clock_t start, stop;
	int i;
	char	szTmp[1024] = {0};

	if ((b = mmap(0, (size+4095)&~4095, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PHYS|MAP_PRIVATE, -1, 0)) == (void *)-1) {
		printf_s("b mmap error");
		return -1;
	}
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// start = myclock();

	for (i = limit; i >= 0; i--){
		write(fd, b, size);
	}
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// stop = myclock();

	sprintf(szTmp, "%d %d %d %d %d\n", start, stop, stop - start, limit, size);
	printf_s(szTmp);
	munmap(b, (size+4095)&~4095);
	return 0;
}

int main( int argc, char *argv[] ) 	{
	int fd;
	int limit;

	limit = 1;//000;	//	calibrate_loop();

	fd = open("/dev/null", FILEOPEN_WRITEONLY);
	clock_gettime(CLOCK_REALTIME, &pstart);
	setprio(0, 29);
	printf_s("%s %s %s %s %s\n", "start", "stop", "diff", "iterations", "bytes");
	testloop(fd, 1, limit);
	testloop(fd, 4, limit);
	//testloop(fd, 1514, limit);
	//testloop(fd, 4096, limit);
	//testloop(fd, 8192, limit);
	//testloop(fd, 63*1024, limit);
	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
