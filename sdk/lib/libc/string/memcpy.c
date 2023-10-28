#include <libc/include/string.h>

void *memcpy(void *dst, const void *src, size_t nbytes) {
	void			*ret = dst;

	/* Both addresses must be aligned to stuff in int size chunks */
	if(		nbytes >= sizeof(unsigned) &&
			((uintptr_t)src & (sizeof(unsigned) - 1)) == 0 &&
			((uintptr_t)dst & (sizeof(unsigned) - 1)) == 0) {
		unsigned			*d = (unsigned *)dst - 1;
		const unsigned		*s = (const unsigned *)src - 1;

		while(nbytes >= sizeof(unsigned)) {
			nbytes -= sizeof(unsigned);
			*++d = *++s;
		}
		if(nbytes) {
			dst = (unsigned char *)(d + 1);
			src = (const unsigned char *)(s + 1);
		}
	}

	/* Get the unaligned bytes, or the remaining bytes */
	while(nbytes) {
		*(unsigned char *)dst = *(const unsigned char *)src;
		dst = (char *)dst + 1;
		src = (const char *)src + 1;
		--nbytes;
	}

	return ret;
}
