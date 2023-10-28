#ifndef   __EPIT_H__
#define   __EPIT_H__

#include "common.h"
#include <platform/arch/arm/include/intr/gic.h>
#include <platform/arch/arm/include/mx6q/MCIMX6Y2.h>
#include <platform/arch/arm/include/mx6q/core_ca9.h>


/* º¯ÊýÉùÃ÷ */
void epit1_init(unsigned int frac, unsigned int value);


/* EPIT - Peripheral instance base addresses */
/** Peripheral EPIT1 base address */
#define EPIT1_BASE                               (0x20D0000u)
/** Peripheral EPIT1 base pointer */
#define EPIT1                                    ((EPIT_Type *)EPIT1_BASE)

#endif

