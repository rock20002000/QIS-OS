/*
 * inout.c
 *
 *  Created by:
 *      Author: Test
 */

#include <libc/include/stdio.h>

uint32_t in32( uintptr_t port )
{
	uint32_t	__data;
	__data = *(volatile uint32_t *)port;
	return(__data);
}


void out32( uintptr_t port, uint32_t val ){
	*(volatile uint32_t *)port = val;
}
