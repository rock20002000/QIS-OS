/*
 * mutex_alloc.c
 *
 *  Created on: 2022-9-16
 *      Author: Test
 */

#include <libc/include/stdio.h>
#include <platform/include/platform.h>

int main(int argc, char *argv[])
{
	int		limit, loop, destroy, opt;
	clock_t		start, stop, init_time, destroy_time;
	char		pass[1024] = {0};
	pthread_mutex_t *mutexs;		//	pthread_mutex_t mutexs[1000];	//

	struct timespec 	pstart;

	//	while(1)
	{
		destroy = 0;
		while ((opt = getopt(argc, argv, "d")) != -1)
		{
			if (opt == 'd')
				destroy = !0;
		}

		//teststart(argv[0]);
		//testpntbegin(argv[0]);

		limit = 100000;	//	calibrate_loop();
		mutexs = malloc ( limit, sizeof( pthread_mutex_t ) );	//	mutexs = calloc ( limit, sizeof( pthread_mutex_t ) );

		clock_gettime(0, &pstart);
		start = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop++ )
		{
			pthread_mutex_init( &mutexs[loop], NULL );
		}
		clock_gettime(0, &pstart);
		stop = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();

		if ( !destroy )
		{
			sprintf( pass, "average mutex create/init = %d usec\n", (stop - start));	//	1000.0 * (double)(stop - start) / (double)limit);
			//	sprintf( pass, "average mutex create/destroy = %3.3f usec", 1000.0 * (double)(stop - start) / (double)limit);

			//	xprintf( pass );
			screen_printf(pass);
		}

		clock_gettime(0, &pstart);
		start = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop++ )
		{
			pthread_mutex_destroy( &mutexs[loop] );
		}
		clock_gettime(0, &pstart);
		stop = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();

		if ( destroy )
		{
			sprintf( pass, "average mutex destroy = %d usec\n", (stop - start));	//		1000.0 * (double)(stop - start) / (double)limit);
			//xprintf( "average mutex destroy = %d usec\n", (stop - start));	//		1000.0 * (double)(stop - start) / (double)limit);

			//	xprintf( pass );
			screen_printf(pass);
		}

		free ( mutexs );
	}



	//testpntpass( pass );
	//testpntend();
	//teststop(argv[0]);
	return EOK;
}
