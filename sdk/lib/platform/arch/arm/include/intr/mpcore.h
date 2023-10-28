#pragma once
/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor registers
 *
 * Register offsets from MPCORE_GIC_BASE
 * -----------------------------------------------------------------------
 */

#define	MPCORE_GIC_DIST_BASE		0x1000	/* offset from SCU base */
#define	MPCORE_GIC_DIST_SIZE		0x1000

/*
 * -----------------------------------------------------------------------
 * Interrupt Distributor CPU interface
 *
 * Register offsets from MPCORE_GIC_BASE
 * -----------------------------------------------------------------------
 */

#define	MPCORE_GIC_CPU_BASE			0x0100	/* offset from SCU base */
#define MPCORE_GIC_CPU_SIZE			0x0100

//! @brief Get base address of private perpherial space.
//!
//! @return The address of the ARM CPU's private peripherals.
uint32_t get_arm_private_peripheral_base(void);
//@}