#pragma once

#include <platform/include/compiler.h>	//	#include "compiler.h"

/* 
C语言实现MCR指令 
*/
#define __MCR(coproc, opcode_1, src, CRn, CRm, opcode_2)                          \
    __ASM volatile ("MCR " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                    "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                    __STRINGIFY(opcode_2)                                         \
                    : : "r" (src) )

/* 
C语言实现MRC指令 
*/                    
#define __MRC(coproc, opcode_1, CRn, CRm, opcode_2)                               \
  ({                                                                              \
    uint32_t __dst;                                                               \
    __ASM volatile ("MRC " __STRINGIFY(p##coproc) ", " __STRINGIFY(opcode_1) ", " \
                    "%0, " __STRINGIFY(c##CRn) ", " __STRINGIFY(c##CRm) ", "      \
                    __STRINGIFY(opcode_2)                                         \
                    : "=r" (__dst) );                                             \
    __dst;                                                                        \
  })


FORCEDINLINE __STATIC_INLINE uint32_t __get_VBAR(void)
{
  return __MRC(15, 0, 12, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_VBAR(uint32_t vbar)
{
  __MCR(15, 0, vbar, 12, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_ISR(void)
{
  return __MRC(15, 0, 12, 1, 0);
}


FORCEDINLINE __STATIC_INLINE void __set_ISR(uint32_t isr)
{
  __MCR(15, 0, isr, 12, 1, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBR0(void)
{
  return __MRC(15, 0, 2, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBR0(uint32_t ttbr0)
{
  __MCR(15, 0, ttbr0, 2, 0, 0);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBR1(void)
{
  return __MRC(15, 0, 2, 0, 1);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBR1(uint32_t ttbr1)
{
  __MCR(15, 0, ttbr1, 2, 0, 1);
}

FORCEDINLINE __STATIC_INLINE uint32_t __get_TTBCR(void)
{
  return __MRC(15, 0, 2, 0, 2);
}

FORCEDINLINE __STATIC_INLINE void __set_TTBCR(uint32_t ttbcr)
{
  __MCR(15, 0, ttbcr, 2, 0, 2);
}


/* Some other C language inline assembly */
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_APSR(uint32_t apsr)
{
  __ASM volatile ("MSR apsr, %0" : : "r" (apsr) : "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_CPSR(void)
{
  uint32_t result;

  __ASM volatile ("MRS %0, cpsr" : "=r" (result) );
  return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_CPSR(uint32_t cpsr)
{
  __ASM volatile ("MSR cpsr, %0" : : "r" (cpsr) : "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_FPEXC(void)
{
  uint32_t result;

  __ASM volatile ("VMRS %0, fpexc" : "=r" (result) );
  return result;
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_FPEXC(uint32_t fpexc)
{
  __ASM volatile ("VMSR fpexc, %0" : : "r" (fpexc));
}


FORCEDINLINE __STATIC_INLINE uint32_t __get_CBAR(void)
{
  return __MRC(15, 4, 15, 0, 0);
}

FORCEDINLINE __STATIC_INLINE void GIC_EnableIRQ(uint32_t IRQn)
{
  	GIC_V2_Type *gic = (GIC_V2_Type *)(__get_CBAR() & 0xFFFF0000UL);
  	gic->D_ISENABLER[((uint32_t)(int32_t)IRQn) >> 5] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}
