/*
 * sprintf.c
 *
 *  Created on: 2022-9-4
 *      Author: Test
 */


#include <libc/include/string.h>

void sprintf(char * buff, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, 0, fmt, args);
    va_end(args);
}
