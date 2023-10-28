#include <libc/include/string.h>

void *memset(void *s, int c, size_t n) {
	char		*p = s;

	/* Stuff unaligned addresses first */
	while(((uintptr_t)p & (sizeof(unsigned) - 1)) && n) {
		*p++ = c;
		n--;
	}

	/* Now stuff in native int size chunks if we can */
	if(n >= sizeof(unsigned)) {
		
#ifdef _KIRK_CONFIG_MACHINE_64BIT_
		unsigned		cc = 0x0101010101010101 * (unsigned char)c;

#else
		unsigned		cc = 0x01010101 * (unsigned char)c;
#endif

		unsigned		*pp = (unsigned *)p - 1;

		while(n >= sizeof(unsigned)) {
			n -= sizeof(unsigned);
			*++pp = cc;
		}
		if(n) {
			p = (char *)(pp + 1);
		}
	}

	/* Get the remaining bytes */
	if(n) {
		p--;
		while(n) {
			n--;
			*++p = c;
		}
	}

	return s;
}
