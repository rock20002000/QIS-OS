#ifndef __BITOPS_H__
#define __BITOPS_H__

#include <stdint.h>

static inline uint8_t ffs(uintmax_t x)
{
	uint8_t num = 0;

#ifdef __LP64__
	if ((x & 0xffffffff) == 0) {
		num += 32;
		x >>= 32;
	}
#endif
	if ((x & 0xffff) == 0) {
		num += 16;
		x >>= 16;
	}
	if ((x & 0xff) == 0) {
		num += 8;
		x >>= 8;
	}
	if ((x & 0xf) == 0) {
		num += 4;
		x >>= 4;
	}
	if ((x & 0x3) == 0) {
		num += 2;
		x >>= 2;
	}
	if ((x & 0x1) == 0)
		num += 1;
	return num;
}

static inline uint8_t fls(uint32_t x)
{
	uint8_t r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline uint8_t get_order(uint32_t count)
{
    if (!count)
        return 0;

    uint32_t order = fls(count) - 1;

    if (count & ((1 << order) - 1))
    {
        order++;
    }
    return order;
}

#endif // __BITOPS_H__
