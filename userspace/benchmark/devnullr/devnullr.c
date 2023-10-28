/*
 * devnullr.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

int
main(int argc, char **argv) {
	int fd;
	int i, j, limit;
	clock_t start, stop;
	char szTmp[1024] = {0};

	limit = 100;//000;	//	calibrate_loop();
	fd = open("/dev/null", FILEOPEN_READONLY);
	clock_gettime(CLOCK_REALTIME, &pstart);
	setprio(0, 29);
	sprintf(szTmp, "%s %s %s %s\n", "start", "stop", "diff", "iterations");
	printf_s(szTmp);

	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// start = myclock();

	for (i = limit; i >= 0; i--){
		read(fd, &j, 1); // always returns -1, end-of-file
	}
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// stop = myclock();

	sprintf(szTmp, "%d %d %d %d\n", start, stop, stop - start, limit);
	printf_s(szTmp);
	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
