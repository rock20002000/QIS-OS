

#ifndef _KLIBC_STRING_H_
#define _KLIBC_STRING_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

// [7.24.2] Copying functions

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size);
void *memmove(void *dest, const void *src, size_t size);
void *memset(void *s, int c, size_t n);

char *strcpy(char *__restrict dest, const char *src);
char *strncpy(char *__restrict dest, const char *src, size_t max_size);

// // [7.24.3] Concatenation functions

char *strcat(char *__restrict dest, const char *__restrict src);
// char *strncat(char *__restrict dest, const char *__restrict src, size_t max_size);

// // [7.24.4] Comparison functions

int memcmp(const void *a, const void *b, size_t size);
int strcmp(const char *a, const char *b);
// int strcoll(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t max_size);
// size_t strxfrm(char *__restrict dest, const char *__restrict src, size_t max_size);

// // [7.24.5] Search functions

// void *memchr(const void *s, int c, size_t size);
const char *strchr(const char *s, int c);
// size_t strcspn(const char *s, const char *chrs);
// char *strpbrk(const char *s, const char *chrs);
// char *strrchr(const char *s, int c);
// size_t strspn(const char *s, const char *chrs);
// char *strstr(const char *pattern, const char *s);
// char *strtok(char *__restrict s, const char *__restrict delimiter);

// // This is a GNU extension.
// char *strchrnul(const char *, int);

// // [7.24.6] Miscellaneous functions

// void *memset(void *dest, int c, size_t size);
// char *strerror(int errnum);
size_t strlen(const char *s);

// // POSIX extensions.

// int strerror_r(int, char *, size_t);
// void *mempcpy(void *, const void *, size_t);

// // GNU extensions.
// int strverscmp(const char *l0, const char *r0);
// int ffsl(long i);
// int ffsll(long long i);
// void *memmem(const void *, size_t, const void *, size_t);


void sprintf(char * buff, const char * fmt, ...);

char* strchrnul(const char *s, int c_in);
size_t strspn(const char *str, const char *accept);
char* strtok_r(char *s, const char *delim, char **save_ptr);
char* strtok(char *s, const char *delim);

#endif

