#include <libc/include/string.h>

size_t strlen(const char *str)
{
    size_t len = 0;

    if (str)
    {
        for (len = 0; str[len]; len++);
    }

    return len;
}
