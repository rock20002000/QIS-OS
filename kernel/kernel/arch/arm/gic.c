/*
 * gic.c
 *
 *  Created on: 2022��12��4��
 *      Author: Test
 */
#include <libc/include/stdio.h>
#include <platform/arch/arm/include/intr/gic.h>
#include <platform/arch/arm/include/mx6q/core_ca9.h>
#include <platform/arch/arm/include/intr/mpcore.h>


uint32_t get_gic_base(void)
{
	uint32_t dst;		//	GIC_Type *dst;

	__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst));
	printf("GIC Base:0x%p\n", dst);
	return dst;
}

static inline gicd_t * gic_get_gicd(void)
{
    uint32_t base = get_arm_private_peripheral_base() + kGICDBaseOffset;
    return (gicd_t *)base;
}

static inline gicc_t * gic_get_gicc(void)
{
    uint32_t base = get_arm_private_peripheral_base() + kGICCBaseOffset;
    return (gicc_t *)base;
}


void gic_set_irq_priority(uint32_t ID, uint32_t priority)
{
    gicd_t * gicd = gic_get_gicd();
    
    // Update the priority register. The priority registers are byte accessible, and the register
    // struct has the priority registers as a byte array, so we can just index directly by the
    // interrupt ID.
    gicd->IPRIORITYRn[ID] = priority & 0xff;
}
