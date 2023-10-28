#include <libc/include/string.h>

char *strcpy(char *dest, const char *src)
{
    size_t i = 0;

    for (; src[i]; i++)
        dest[i] = src[i];

    dest[i] = 0;

    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i = 0;

    for (; i < n-1 && src[i]; i++)
        dest[i] = src[i];

    dest[i] = 0;

    return dest;
}
