/*
 * thread.c
 *
 *  Created on: 2022Äê12ÔÂ26ÈÕ
 *      Author: Test
 */



#include <platform/include/platform.h>

void test_wait(void *blah) {
	xprintf("sleep\n");
	sleep(3);
	xprintf("exit");
	return;
}

void *
test_print(void *blah) {
	for (;;) {
		xprintf("%s\n", (char*)blah);;//write(1, blah, 1);
	}
	return 0;
}

int
main(int argc, char **argv) {
	pthread_t tid, tid2;
	//	pthread_t *tid  = malloc(sizeof(pthread_t));
	//	pthread_t *tid2 = malloc(sizeof(pthread_t));;
	pthread_create(&tid, 0, test_wait, "a");
	xprintf("pthread_join tid:%d\n", tid);
	pthread_join(tid, 0);
	xprintf("pthread_join done\n");

	pthread_create(&tid2, 0, test_wait, "b");
	xprintf("pthread_join tid2:%d\n", tid2);
	pthread_join(tid2, 0);
	xprintf("pthread_join done 2\n");


	return 0;
}


