#include <libc/include/string.h>

char *strcat(char* dest, const char* src)
{
    if (!dest)
        return src;
    if (!src)
        return dest;

    size_t len = strlen(dest);

    for (int i = 0; src[i]; i++, len++)
        dest[len] = src[i];

    dest[len] = 0;

    return dest;
}
