/*
 * speed.c
 *
 *  Created on: 2022-9-5
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <platform/include/time.h>

#define LOOP_COUNT 2000000

struct timespec pstart;

volatile unsigned int keeploop = 0;

int main(int argc, char *argv[]) {
	clock_t		start, stop;
	int			loop, limit;
	char		szTmp[1024] = {0};
	limit = LOOP_COUNT;	//	calibrate_loop();

	//while(1){
		clock_gettime(0, &pstart);	//	CLOCK_REALTIME, &pstart);
		start = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	myclock();
		for (loop = limit; loop > 0; loop--)
			++keeploop;

		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;				//	myclock();
		//	xprintf("%s %s %s %s\n", "start", "stop", "diff", "loops");			//	xprintf("%10s %10s %10s %10s\n", "start", "stop", "diff", "loops");
		//	xprintf("%d %d %d %d\n", start, stop, stop - start, limit);			//	xprintf("%10u %10u %10u %10d\n", start, stop, stop - start, limit);

		sprintf(szTmp, "%s %s %s %s\n", "start", "stop", "diff", "loops");			//	xprintf("%10s %10s %10s %10s\n", "start", "stop", "diff", "loops");
		screen_printf(szTmp);
		sprintf(szTmp, "%d %d %d %d\n", start, stop, stop - start, limit);			//	xprintf("%10u %10u %10u %10d\n", start, stop, stop - start, limit);
		screen_printf(szTmp);
	//}



	return 0;
}



