/*
 * slicing.c
 *
 *  Created on: 2022-9-6
 *      Author: Test
 */

#include <platform/include/platform.h>
//	#include <unistd.h>
//	#include <pthread.h>

void *
test_print(void *blah) {
	for (;;) {
		xprintf("%s\n", (char*)blah);;//write(1, blah, 1);
	}
	return 0;
}

int
main(int argc, char **argv) {
	pthread_create(0, 0, test_print, "a");
	pthread_create(0, 0, test_print, "b");
	while(1){
		xprintf("c\n");
	}
	return 0;
}

