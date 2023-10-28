#include <libc/include/stdio.h>
#include <platform/arch/arm/include/intr/mpcore.h>
#include "stdint.h"
/*
 * The board specific startup MUST set this to the SCU address
 */
unsigned mpcore_scu_base = 0x00a00000;

void armv_detect_a9(void)
{
	unsigned	scu;

	/*
	 * Figure out if we're an MPCore implementation by reading the
	 * Configuration Base Address register
	 */
	__asm__ __volatile__("mrc	p15, 4, %0, c15, c0, 0" : "=r"(scu));


	/*
	 * MPCore implementation
	 */
	mpcore_scu_base = scu;
	// printf("mpcore_scu_base: 0x%p\n",mpcore_scu_base);
}

void arm_gic_init(paddr_t dist_base, paddr_t cpu_base);

void init_intrinfo()
{
	unsigned	gic_dist;
	unsigned	gic_cpu ;
	armv_detect_a9();
	/* mpcore_scu_base is set in function armv_detect_a9 */
	gic_dist = mpcore_scu_base + MPCORE_GIC_DIST_BASE;
	gic_cpu  = mpcore_scu_base + MPCORE_GIC_CPU_BASE;
		printf("gic_dist: 0x%p\n",gic_dist);
		printf("gic_cpu: 0x%p\n",gic_cpu);
	/*
	 * Initialise GIC distributor and our cpu interface
	 */
	arm_gic_init(gic_dist, gic_cpu);

	//add_interrupt_array(intrs, sizeof(intrs));
}

