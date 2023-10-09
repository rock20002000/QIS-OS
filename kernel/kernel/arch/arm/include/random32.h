#pragma once
#include <debug.h>

static const int RETRY_COUNT = 10;

// Some Definitions
#define RDRAND_SUPPORTED 3
#define RDRAND_NOT_SUPPORTED 2
#define RDRAND_SUCCESS 1
#define RDRAND_FAIL 0

// Intel recommends the maximum number of retries should be 10
// If the rdrand instruction fails 10 consecutive times then that
// indicates there is a larger problem with the processor
#define DEFAULT_RETRY_LIMIT 10

//	int g_iRandomValue = 0;
static inline void fill_random_buf(uintmax_t *buf, unsigned long RANDOM_COUNT) {
}

//	static inline uint32_t get_random_long(void) { return 0;}	//	g_iRandomValue++; }
//	static inline uint32_t get_random_int(void) { return 0;}	//	g_iRandomValue++; }

uint32_t get_random_long(void);
uint32_t get_random_int(void);
