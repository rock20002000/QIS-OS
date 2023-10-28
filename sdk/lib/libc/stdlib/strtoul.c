#include <types.h>
#include <libc/include/ctype.h>

#define hexstr(p) (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))

static inline int radix_value(char c)
{
	if(c >= '0'  &&  c <= '9')
        return(c - '0');
	if(c >= 'a'  &&  c <= 'f')
        return(c - 'a' + 10);
	if(c >= 'A'  &&  c <= 'F')
        return(c - 'A' + 10);
	return 37;
}

unsigned long strtoul(const char *nptr, char **endptr, int base)
{
	int             digit;
	const char      *p;
	const char      *startp;
	unsigned long   value;

	if(endptr != NULL)
        *endptr = (char *)nptr;
	p = nptr;
	if(base == 0)
    {
	    if(hexstr(p))
            base = 16;
	    else if(*p == '0')
            base = 8;
	    else
            base = 10;
	}
	if(base == 16)
    {
	    if(hexstr(p))
            p += 2;	/* skip over '0x' */
	}
	startp = p;
	value = 0;
	while (1)
    {
	    digit = radix_value(*p);
	    if(digit >= base)
            break;
	    value = value * base + digit;
	    ++p;
	}
	if(p == startp)
        p = nptr;
	if(endptr != NULL)
        *endptr = (char *)p;
	return value;
}

int atoi(const char *nptr)
{
	int             neg = 0;

    while (*nptr && isspace(*nptr))
        nptr++;
    if ('-' == *nptr)
    {
        nptr++;
        neg = 1;
    }

    unsigned long value = strtoul(nptr, NULL, 0);

    return neg ? -((int)value) : (int)value;
}
