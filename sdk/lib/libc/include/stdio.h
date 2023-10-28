#ifndef __KLIBC_VPRINTF_H__
#define __KLIBC_VPRINTF_H__

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef intmax_t    fsize_t;
typedef intmax_t    fpos_t;

typedef struct
{
    int         fid;
    int         mode;
    int         stat;
    fsize_t     size;
    offset_t    offs;
} file_t;

typedef file_t      FILE;

#define SEEK_SET    0   /*  Seek relative to start of file  */
#define SEEK_CUR    1   /*  Seek relative to current positn */
#define SEEK_END    2   /*  Seek relative to end of file    */

extern FILE* fopen (const char* filename, const char* mode);
extern size_t fread (void* ptr, size_t size, size_t n, FILE* file);
extern size_t fwrite (const void* ptr, size_t size, size_t n, FILE* file);
extern int fseek (FILE* FILE, fpos_t off, int whence);
extern fpos_t ftell (FILE* FILE);
extern int fflush (FILE* file);
extern int fclose (FILE* file);


struct vprintf_context {
    void (*printchar)(struct vprintf_context *ctx, char ch);
    char *buf;
    size_t size;
    size_t index;
    void* fd;
};


/*vprintf.c*/
void vprintf_with_context(struct vprintf_context *ctx, const char *fmt,
                          va_list vargs);
void printf_with_context(struct vprintf_context *ctx, const char *fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list vargs);
int snprintf(char *buf, size_t size, const char *fmt, ...);

/*printf.c*/
__weak void printchar(__unused struct vprintf_context *ctx, char ch);
int printf(const char *fmt, ...);

//void printf(const char *fmt, ...);

void printf_s(const char *fmt, ...);
void perror(const char *pszOut);	//	void perror(const char *fmt, ...);

uint32_t in32( uintptr_t port );
void out32( uintptr_t port, uint32_t val );

extern char *optarg;
extern int getopt(int argc, char * const argv[], const char *optstring);

#endif
