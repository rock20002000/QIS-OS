/*
 * atomic.h
 *
 *  Created on: 2022-9-20
 *      Author: Test
 */

#ifndef SDK_LIB_LIBC_ATOMIC_ARCH_X86_64_ATOMIC_H_
#define SDK_LIB_LIBC_ATOMIC_ARCH_X86_64_ATOMIC_H_

extern void atomic_add(volatile unsigned *__loc, unsigned __incr);
extern void atomic_clr(volatile unsigned *__loc, unsigned __bits);
extern void atomic_set(volatile unsigned *__loc, unsigned __bits);
extern void atomic_sub(volatile unsigned *__loc, unsigned __decr);
extern void atomic_toggle(volatile unsigned *__loc, unsigned __bits);

extern unsigned atomic_add_value(volatile unsigned *__loc, unsigned __incr);
extern unsigned atomic_clr_value(volatile unsigned *__loc, unsigned __bits);
extern unsigned atomic_set_value(volatile unsigned *__loc, unsigned __bits);
extern unsigned atomic_sub_value(volatile unsigned *__loc, unsigned __decr);
extern unsigned atomic_toggle_value(volatile unsigned *__loc, unsigned __bits);


#endif /* SDK_LIB_LIBC_ATOMIC_ARCH_X86_64_ATOMIC_H_ */
