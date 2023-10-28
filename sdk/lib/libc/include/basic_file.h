/*
 * basic_file.h
 *
 *  Created by:
 *      Author: Test
 */

#ifndef SDK_LIB_LIBC_INCLUDE_BASIC_FILE_H_
#define SDK_LIB_LIBC_INCLUDE_BASIC_FILE_H_

#include <libc/include/iomsg.h>

#define FILEOPEN_READONLY    	0x1
#define FILEOPEN_WRITEONLY    	0x2
#define FILEOPEN_READWRITE     	0x3

int open(const char *path, int oflag, ...);
size_t read(int fd, void *buff, size_t nbytes);
size_t write(int fd, const void *buff, size_t nbytes);
int devctl(int fd, int device_cmd, void *data, size_t nbytes, int *info);
int close(int fd);

int name_open( const char * name, int flags );

#endif /* SDK_LIB_LIBC_INCLUDE_BASIC_FILE_H_ */
