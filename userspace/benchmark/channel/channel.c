/*
 * channel.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <platform/include/resmgr.h>

#define LIMIT_DIV 100

int main(int argc, char *argv[])
{
	int		volatile limit, loop, *chid, opt;
	clock_t		start, stop, destroy;
	struct timespec pstart;
	char		pass[80];

	destroy = 0;
	//while ((opt = getopt(argc, argv, "d")) != -1) {
	//	if (opt == 'd')
	//		destroy = !0;
	//}

	//teststart(argv[0]);
	//testpntbegin(argv[0]);

	limit = 1000;	//	calibrate_loop() / LIMIT_DIV;

	chid = malloc(limit*sizeof(int));	//calloc( limit, sizeof( int ) );
	memset((char*)chid, 0, limit*sizeof(int));

	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
	for (loop = 0; loop < limit; loop++)
	{
		chid[loop] = ChannelCreate(NULL);
	}
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	if ( !destroy )
	{
		sprintf( pass, "average channel create = %d usec", (int)(1000.0 * (stop - start) / limit) );	//	sprintf( pass, "average channel create = %3.3f usec", 1000.0 * (double)(stop - start) / (double)limit );
		printf_s(pass);
	}

	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
	for (loop = 0; loop < limit; loop++)
	{
		ChannelDestroy( chid[loop] );
	}

	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	if ( destroy )
	{
		sprintf( pass, "average channel destroy = %d usec", (int)(1000.0 * (stop - start) / limit) );	//	sprintf( pass, "average channel destroy = %3.3f usec", 1000.0 * (double)(stop - start) / (double)limit );
		printf_s(pass);
	}

	free( chid );

	//	testpntpass(pass);
	//	testpntend();
	//	teststop(argv[0]);
	return EOK;	//EXIT_SUCCESS;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/

