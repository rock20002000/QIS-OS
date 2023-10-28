#include <types.h>
#include <libc/include/stdio.h>
#include <platform/arch/arm/include/intr/gic.h>

paddr_t	gic_dist_base;
paddr_t	gic_cpu_base;

void arm_gic_cpu_init()
{
// extern void ser_putstr(const char *str);
	if (gic_dist_base == 0 || gic_cpu_base == 0) {
		printf("gic not initialised");
	}

	/*
	 * Disable all banked PPI interrupts
	 * Enable all SGI interrupts
	 */
	out32(gic_dist_base + ARM_GICD_ICENABLERn, 0xffff0000);
	out32(gic_dist_base + ARM_GICD_ISENABLERn, 0x0000ffff);

	/*
	 * Set priority mask to allow all interrupts and enable cpu interface
	 */
	out32(gic_cpu_base + ARM_GICC_PMR, 0xF0);
	out32(gic_cpu_base + ARM_GICC_CTLR, ARM_GICC_CTLR_EN);
}

void arm_gic_init(paddr_t dist_base, paddr_t cpu_base)
{
	unsigned	i;
	unsigned	itn;

	gic_dist_base = dist_base;
	gic_cpu_base = cpu_base;

	/*
	 * Make sure distributor is disabled
	 */
	out32(dist_base + ARM_GICD_CTLR, 0);

	/*
	 * Calculate number of interrupt lines
	 */
	itn = ((in32(dist_base + ARM_GICD_TYPER) & ARM_GICD_TYPER_ITLN) + 1) * 32;

	/*
	 * Disable all interrupts and clear pending state
	 */
	for (i = 0; i < itn; i += 32) {
		out32(dist_base + ARM_GICD_ICENABLERn + (i * 4 / 32), 0xffffffff);
		out32(dist_base + ARM_GICD_ICPENDRn + (i * 4 / 32), 0xffffffff);
	}

	/*
	 * Set default priority of all interrupts to 0xa0
	 */
	for (i = 0; i < itn; i += 4) {
		out32(dist_base + ARM_GICD_IPRIORITYn + i, 0xa0a0a0a0);
	}

	/*
	 * Route all SPI interrupts to cpu0
	 */
	for (i = 32; i < itn; i += 4) {
		out32(dist_base + ARM_GICD_ITARGETSRn + i, 0x01010101);
	}

	/*
	 * Default all SPI interrupts as level triggered
	 */
	for (i = 32; i < itn; i += 16) {
		out32(dist_base + ARM_GICD_ICFGRn + (i * 4 / 16), 0);
	}

	/*
	 * Enable distributor
	 */
	out32(dist_base + ARM_GICD_CTLR, ARM_GICD_CTLR_EN);

	/*
	 * Enable cpu interface for cpu0.
	 * Secondary cpu interfaces are enables as those cpus are initialised
	 */
	arm_gic_cpu_init();
}

