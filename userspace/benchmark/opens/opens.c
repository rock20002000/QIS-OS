/*
 * opens.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

int main(int argc, char **argv) {
	char *target;
	int i, limit;
	clock_t start, stop;
	char	buf[80];

	target = "/dev/null";
	sprintf(buf, "Open %s\n", target);
	printf_s(buf);

	limit = 100;
	clock_gettime(CLOCK_REALTIME, &pstart);
	setprio(0, 29);
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
	for (i = limit; i >= 0; i--)
		close(open(target, FILEOPEN_READONLY));
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
	sprintf(buf, "Avg time for open call = %d usec\n", (int)(1000.0 * (double)(stop - start) / (double) limit));
	printf_s(buf);

	printf_s("Open /doesnotexist\n");
	clock_gettime(CLOCK_REALTIME, &pstart);
	setprio(0, 29);
	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
	for (i = limit; i >= 0; i--)
		close(open("/doesnotexist", FILEOPEN_READONLY));
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
	sprintf(buf, "Avg time for open call = %d usec\n", (int)(1000.0 * (double)(stop - start) / (double) limit));
	printf_s(buf);

	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
