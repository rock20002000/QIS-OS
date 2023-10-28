/*
 * rand.c
 *
 *  Created by:
 *      Author: Test
 */

int rand( void ){
	int        ret = 0;

	__asm__ __volatile__("");

	return ret;
}

void srand( unsigned int seed ){
	return;
}
