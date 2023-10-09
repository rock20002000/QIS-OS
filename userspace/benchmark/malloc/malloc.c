/*
 * malloc.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <libc/include/stdio.h>
#include <platform/include/resmgr.h>
#include <platform/include/rand.h>

#define DEFAULT_MEM_SIZE 1024*4
// #define DEFAULT_MEM_SIZE 256
#define	DEFAULT_ARRAY_LEN 16
#define LIMIT_DIV 100

#ifndef MALLOC_TEST_LIMIT
 #define MALLOC_TEST_LIMIT	((0x40000000 - 0x0808000) / (DEFAULT_MEM_SIZE * DEFAULT_ARRAY_LEN))
#endif

enum { BENCH_MALLOC, BENCH_CALLOC, BENCH_REALLOC };
int main(int argc, char *argv[])
{
	int				mode, opt, limit, loop, length, randsize, destroy;
	size_t			size;
	char			pass[120];
	void			**mem_ptrs;
	clock_t			start, stop, alloc_time, free_time;
	struct 			timespec pstart;
	//teststart(argv[0]);
	//testpntbegin(argv[0]);

	randsize = 0;
	mode = BENCH_MALLOC;
	size = DEFAULT_MEM_SIZE;
	length = DEFAULT_ARRAY_LEN;
	destroy = 0;

	while ((opt = getopt(argc, argv, "mcRrn:s:d")) != -1)
	{
		if (opt == 's')
			size = atoi(optarg);
		else if (opt == 'm')
			mode = BENCH_MALLOC;
		else if (opt == 'c')
			mode = BENCH_CALLOC;
		else if (opt == 'r')
			mode = BENCH_REALLOC;
		else if (opt == 'n')
			length = atoi(optarg);
		else if (opt == 'R')
		{
			randsize = !0;
			srand( time(NULL) );
		}
		else if (opt == 'd')
			destroy = !0;
	}
	limit = 10000;	//	calibrate_loop() / LIMIT_DIV;

	mem_ptrs = malloc(limit*sizeof(void*));	//	calloc( limit, sizeof(void*) );
	memset((char*)mem_ptrs, 0, limit*sizeof(void*));

	switch (mode)
	{
	case BENCH_MALLOC:
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// start = myclock();

		for ( loop = 0; loop < limit; loop ++ )
		{
			mem_ptrs[loop] = malloc( randsize? rand() : size );
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		alloc_time = stop - start;

		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			free( mem_ptrs[loop] );
			xprintf("malloc-free,current loop number is %d \r\n",loop);
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		free_time = stop - start;
		break;
	case BENCH_CALLOC:
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			mem_ptrs[loop] = calloc(length, randsize? rand() : size );
			// memset( (char*)mem_ptrs[loop], 0, length* (randsize? rand() : size));	// because calloc done memset 0 alredy
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		alloc_time = stop - start;

		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			free( mem_ptrs[loop] );
			xprintf("calloc-free,current loop number is %d \r\n",loop);
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		free_time = stop - start;
		break;
	case BENCH_REALLOC:
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			size = randsize? rand() : size;
			mem_ptrs[loop] = realloc( NULL, size );
			mem_ptrs[loop] = realloc( mem_ptrs[loop], size/2 );
			mem_ptrs[loop] = realloc( mem_ptrs[loop], size*2 );
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		alloc_time = stop - start;

		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			realloc( mem_ptrs[loop], 0 ); //free()
			xprintf("realloc-free,current loop number is %d \r\n",loop);
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		free_time = stop - start;
		break;
	default:
		perror("Invalid mode");	//	testerror("Invalid mode");
		break;
	}

	free ( mem_ptrs );

	if ( mode == BENCH_CALLOC )
	{
		if ( !destroy )
		{
			sprintf(pass, "average calloc (%d elements of %d bytes) = %d usec", length, size, (int)(1000.0 * (double)(alloc_time) / (double)limit) );	//	sprintf(pass, "average calloc (%d elements of %d bytes) = %3.3f usec", length, size, 1000.0 * (double)(alloc_time) / (double)limit );
		}
		else
		{
			sprintf(pass, "average free (%d elements of %d bytes) = %3.3f usec", length, size, 1000.0 * (double)(free_time) / (double)limit );	//	sprintf(pass, "average free (%d elements of %d bytes) = %3.3f usec", length, size, 1000.0 * (double)(free_time) / (double)limit );
		}
		printf_s(pass);	//	testpntpass(pass);
	}
	else
	{
		if ( !destroy )
		{
			sprintf(pass, "average %s (%d bytes) = %d usec", (mode == BENCH_MALLOC) ? "malloc" : (mode == BENCH_REALLOC) ? "realloc(initial malloc)+realloc(resize/2)+realloc(resize*2)" : "unknown" , size, (int)(1000.0 * (double)(alloc_time) / (double)limit) );	//	sprintf(pass, "average %s (%d bytes) = %3.3f usec", (mode == BENCH_MALLOC) ? "malloc" : (mode == BENCH_REALLOC) ? "realloc(initial malloc)+realloc(resize/2)+realloc(resize*2)" : "unknown" , size, 1000.0 * (double)(alloc_time) / (double)limit );
		}
		else
		{
			sprintf(pass, "average %s (%d bytes) = %d usec", (mode == BENCH_MALLOC) ? "free" : (mode == BENCH_REALLOC) ? "realloc(free)" : "unknown" , size, (int)(1000.0 * (double)(free_time) / (double)limit) );	//	sprintf(pass, "average %s (%d bytes) = %3.3f usec", (mode == BENCH_MALLOC) ? "free" : (mode == BENCH_REALLOC) ? "realloc(free)" : "unknown" , size, 1000.0 * (double)(free_time) / (double)limit );
		}
		printf_s(pass);	//	testpntpass(pass);
	}
	//	testpntend();
	//	teststop(argv[0]);
	return	EOK;	// EXIT_SUCCESS;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
