/*
 * cache.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

#define MAP_FLAGS_TEST	(MAP_LAZY | MAP_PRIVATE)
#define TEST_SIZE       (4 * 1024 * 1024)

int
main(int argc, char *argv[]) {
	clock_t		start, stop;
	struct timespec pstart;
	int			loop, limit;
	void		*p;
	int			read = 0;
	int			write = 0;
	int			cache = 0;
	int			size = 0;

	//	if(argc > 1) {
	//	char	*arg = argv[1];
	//	size = strtoul(arg, &arg, 0);
	//	while(*arg) {
	//		switch(*arg) {
	//		case 'r':
	//		case 'R':
	//			read = 1;
	//			break;
	//		case 'w':
	//		case 'W':
	//			write = 1;
	//			break;
	//		case 'c':
	//		case 'C':
	//			cache = 1;
	//			break;
	//		}
	//		++arg;
	//	}
	//}
	if(!read && !write) {
		read = 1;
		write = 1;
	}
	if(size == 0) {
		size = 1;
	}

	p = mmap( NULL, TEST_SIZE, PROT_READ_WRITE, MAP_FLAGS_TEST, NOFD, 0);	//0x1000, PROT_READ | PROT_WRITE | (!cache ? PROT_NOCACHE : 0), MAP_ANON | MAP_PRIVATE, NOFD, 0 );
	if(p == MAP_FAILED) {
		printf_s("can not obtain test memory\n");	//	fprintf(stderr, "can not obtain test memory\n");
		return 1;
	}

	limit = 1000;	//calibrate_loop() << 4;

	printf_s("%s %s %s %s\n", "start", "stop", "diff", "loops");
	clock_gettime(CLOCK_REALTIME, &pstart);
	if(read) {
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// start = myclock();

		for (loop = limit; loop > 0; loop--) {
			int		dummy;

			switch(size) {
			case 1:
				dummy = *(volatile uint8_t *)p;
				break;
			case 2:
				dummy = *(volatile uint16_t *)p;
				break;
			case 4:
				dummy = *(volatile uint32_t *)p;
				break;
			case 8:
				dummy = *(volatile uint64_t *)p;
				break;
			}
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	stop = myclock();

		printf_s("%d %d %d %d read  %d\n", start, stop, stop - start, limit, size);
	}
	if(write) {
		clock_gettime(0, &pstart);
		start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	start = myclock();
		for (loop = limit; loop > 0; loop--) {
			switch(size) {
			case 1:
				*(volatile uint8_t *)p = 1;
				break;
			case 2:
				*(volatile uint16_t *)p = 1;
				break;
			case 4:
				*(volatile uint32_t *)p = 1;
				break;
			case 8:
				*(volatile uint64_t *)p = 1;
				break;
			}
		}
		clock_gettime(0, &pstart);
		stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	// 	stop = myclock();
		printf_s("%10u %10u %10u %10d write %d\n", start, stop, stop - start, limit, size);
	}
	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
