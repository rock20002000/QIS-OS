/*
 * arch_common.h
 *
 *  Created on: 2022Äê11ÔÂ14ÈÕ
 *      Author: Test
 */

#ifndef SDK_LIB_PLATFORM_ARCH_ARM_INCLUDE_ARCH_COMMON_H_
#define SDK_LIB_PLATFORM_ARCH_ARM_INCLUDE_ARCH_COMMON_H_

#include <types.h>

static inline uint32_t xchg32(void * dword_addr, uint32_t exchange_value)
{
    uint32_t old_value = 0;
    __asm__ volatile ("");
    return old_value;
}

static inline unsigned char in8(unsigned int port)
{
   unsigned char ret = *(volatile uint8_t *)port;
   return ret;
}

static inline void out8(unsigned int port, unsigned char szMsgTmp)
{
	*(volatile uint8_t *)port = szMsgTmp;
	return;
}


#endif /* SDK_LIB_PLATFORM_ARCH_ARM_INCLUDE_ARCH_COMMON_H_ */
