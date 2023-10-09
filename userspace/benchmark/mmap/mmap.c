/*
 * mmap.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/stdio.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

#define DEFAULT_MEM_SIZE 256
#define LIMIT_DIV 100

int main(int argc, char *argv[])
{
	int				opt, limit, loop, flags, randsize, destroy;
	size_t				size;
	char				pass[80];
	void				**mem_ptrs, *addr;
	clock_t				start, stop, map_time, unmap_time;

	//teststart(argv[0]);
	//testpntbegin(argv[0]);

	randsize = 0;
	size = DEFAULT_MEM_SIZE;
	flags = MAP_PRIVATE;
	addr = NULL;
	destroy = 0;

	while ((opt = getopt(argc, argv, "s:hf:laRd" )) != -1)
	{
		if (opt == 's')
			size = atoi(optarg);
		if (opt == 'R')
		{
			randsize = !0;
			srand( time( NULL ) );
		}
		if (opt == 'h')
		{
			flags &= !MAP_PRIVATE; //mask MAP_PRIVATE
			flags |= MAP_SHARED;
		}
		if (opt == 'f')
		{
			flags |= MAP_FIXED;
			addr = (void *)atoi(optarg);
		}
		if (opt == 'l')
			flags |= MAP_LAZY;
		if (opt == 'a')
			flags |= MAP_ANON;
		if (opt == 'd')
			destroy = !0;
	}
	limit = 100;	//	calibrate_loop() / LIMIT_DIV;

	mem_ptrs = calloc( limit, sizeof(void *));

	if ( !randsize )
	{
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			mem_ptrs[loop] = mmap( addr, size, 0, flags, NOFD, 0);
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		map_time = stop - start;
	}
	else
	{
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for ( loop = 0; loop < limit; loop ++ )
		{
			mem_ptrs[loop] = mmap( addr, rand(), 0, flags, NOFD, 0);
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
		map_time = stop - start;
	}

	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
	for ( loop = 0; loop < limit; loop ++ )
	{
		munmap( mem_ptrs[loop], size );
	}
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();
	unmap_time = stop - start;

	free ( mem_ptrs );

	if ( !destroy )
	{
		sprintf(pass, "average mmap (%d bytes) = %d usec", size, (int)(1000.0 * (double)(map_time) / (double)limit) );	//	sprintf(pass, "average mmap (%d bytes) = %3.3f usec", size, 1000.0 * (double)(map_time) / (double)limit );
	}
	else
	{
		sprintf(pass, "average munmap (%d bytes) = %d usec", size, (int)(1000.0 * (double)(unmap_time) / (double)limit) );	//	sprintf(pass, "average munmap (%d bytes) = %3.3f usec", size, 1000.0 * (double)(unmap_time) / (double)limit );
	}
	printf_s(pass);
	//testpntpass(pass);
	//testpntend();
	//teststop(argv[0]);
	return EOK;	//	EXIT_SUCCESS;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/

