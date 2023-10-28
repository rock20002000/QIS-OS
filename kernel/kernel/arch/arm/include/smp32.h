#pragma once

#include  <types.h>
#include <print_macros.h>
#include "boot/include/bootinfo.h"

/*
    SCU register Offsets from platform-specific SCU base address
*/

#define MPCORE_SCU_CTRL                0x0  /*SCU control Register*/
#define MPCORE_SCU_CONFIG              0x4  /*SCU configuration Register*/
#define MPCORE_SCU_STATUS              0x8  /*SCU cpu status Register*/
#define MPCORE_SCU_INVALIDATE          0xC  /*SCU invalidate all Register*/
#define MPCORE_SCU_DIAGNOSTIC          0x30  /*SCU DIAGNOSTIC Register*/


/*
 * SCU_CONFIG bits
 */
#define	MPCORE_SCU_CONFIG_CPUNUM_MASK	0x00000003
#define	MPCORE_SCU_CONFIG_SMP_MASK		0x000000f0

#define	MPCORE_SCU_CONFIG_TAG_RAM_SIZE(config, cpu) \
		(((config) >> (8 + ((cpu) * 2))) & 3)
#define	MPCORE_SCU_CONFIG_TAG_RAM_16K	0
#define	MPCORE_SCU_CONFIG_TAG_RAM_32K	1
#define	MPCORE_SCU_CONFIG_TAG_RAM_64K	2
#define	MPCORE_SCU_CONFIG_TAG_RAM_RSVD	3

/*
 * SCU_CPU_STATUS bits
 */
#define	MPCORE_SCU_CPU_STATUS(status, cpu) \
		(((status) >> ((cpu) * 2)) & 3)
#define	MPCORE_SCU_CPU_STATUS_NORMAL	0
#define	MPCORE_SCU_CPU_STATUS_RSVD		1
#define	MPCORE_SCU_CPU_STATUS_DORMANT	2
#define	MPCORE_SCU_CPU_STATUS_OFF		2


/*
 * SCU_INVALIDATE bits
 */
#define	MPCORE_SCU_INVALIDATE_BITS(cpu, ways) \
		(((ways) & 0xf) << ((cpu) * 4))

/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor registers
 *
 * Register offsets from MPCORE_GIC_BASE
 * -----------------------------------------------------------------------
 */

#define	MPCORE_GIC_DIST_BASE		0x1000	/* offset from SCU base */
#define	MPCORE_GIC_DIST_SIZE		0x1000

#define	MPCORE_GIC_DIST_CTRL		0x000	/* distributor control */
#define	MPCORE_GIC_DIST_TYPE		0x004
#define	MPCORE_GIC_DIST_ENABLE_SET	0x100
#define	MPCORE_GIC_DIST_ENABLE_CLR	0x180
#define	MPCORE_GIC_DIST_PENDING_SET	0x200
#define	MPCORE_GIC_DIST_PENDING_CLR	0x280
#define	MPCORE_GIC_DIST_ACTIVE_BIT	0x300
#define	MPCORE_GIC_DIST_PRIORITY	0x400
#define	MPCORE_GIC_DIST_TARGET		0x800
#define	MPCORE_GIC_DIST_CONFIG		0xc00
#define	MPCORE_GIC_DIST_SOFTINT		0xf00

#define	MPCORE_GIC_DIST_CTRL_EN		0x00000001


/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor CPU interface
 *
 * Register offsets from MPCORE_GIC_BASE
 * -----------------------------------------------------------------------
 */

#define	MPCORE_GIC_CPU_BASE			0x0100	/* offset from SCU base */
#define MPCORE_GIC_CPU_SIZE			0x0100

#define	MPCORE_GIC_CPU_CTRL			0x00
#define	MPCORE_GIC_CPU_PRIO_MASK	0x04
#define	MPCORE_GIC_CPU_BINARY_POINT	0x08
#define	MPCORE_GIC_CPU_IACK			0x0c
#define	MPCORE_GIC_CPU_EOI			0x10
#define	MPCORE_GIC_CPU_RUNNING_PRI	0x14
#define	MPCORE_GIC_CPU_HIGHEST_PRI	0x18

/*
 * CPU_CTRL bits
 */
#define	MPCORE_GIC_CPU_CTRL_EN		0x00000001

/*
 * CPU_IACK bits
 */
#define	MPCORE_GIC_CPU_IACK_SRCID	0x00001c00

static unsigned mpcore_scu_base = 0x00a00000;

uint8_t debug_flag = 1;

typedef struct {
	unsigned int pending;
	void* start_addr;
	unsigned int cpu;
	void (*send_ipi)(void* data);
} smp_cpu_t;

typedef struct {
	smp_cpu_t  storage[4];
	unsigned int storage_size;
	unsigned int length;
} smp_entry_t;
vaddr_t arch_map_iospace(paddr_t paddr, size_t size, bool user);
static inline int board_smp_num_cpu(void)
{
//	g_gicvaddr = arch_map_iospace(nGicAddr, 0x4000, false);
	vaddr_t mpcore_scu_base_v =  (vaddr_t)arch_map_iospace(mpcore_scu_base, sizeof mpcore_scu_base, false);
	 int cpunum  = in32(mpcore_scu_base_v + MPCORE_SCU_CONFIG);
	 if (debug_flag)
	 		DBG("\n ******SCU_CONFIG = %X %d Cpus******\n", cpunum, (cpunum & 0xf) +1);
	 return (cpunum & 0xf) + 1;
}