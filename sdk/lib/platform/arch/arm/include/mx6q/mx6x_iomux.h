#ifndef	SDK_LIB_INCLUDE_ARM_MX6X_IOMUX_H
#define	SDK_LIB_INCLUDE_ARM_MX6X_IOMUX_H

#include "mx6dq_iomux.h"
#include "mx6sdl_iomux.h"

#define MUX_CTL_SION					(0x1 << 4)
#define MUX_CTL_MUX_MODE_ALT0			0
#define MUX_CTL_MUX_MODE_ALT1			1
#define MUX_CTL_MUX_MODE_ALT2			2
#define MUX_CTL_MUX_MODE_ALT3			3
#define MUX_CTL_MUX_MODE_ALT4			4
#define MUX_CTL_MUX_MODE_ALT5			5
#define MUX_CTL_MUX_MODE_ALT6			6
#define MUX_CTL_MUX_MODE_ALT7			7

#define PAD_CTL_HYS_DISABLE				(0x0 << 16)
#define PAD_CTL_HYS_ENABLE				(0x1 << 16)

#define PAD_CTL_PUS_100K_PD				(0x0 << 14)
#define PAD_CTL_PUS_47K_PU				(0x1 << 14)
#define PAD_CTL_PUS_100K_PU				(0x2 << 14)
#define PAD_CTL_PUS_22K_PU				(0x3 << 14)

#define PAD_CTL_PUE_PULL				(0x1 << 13)
#define PAD_CTL_PUE_KEEP				(0x0 << 13)

#define PAD_CTL_PKE_DISABLE				(0x0 << 12)
#define PAD_CTL_PKE_ENABLE				(0x1 << 12)

#define PAD_CTL_ODE_DISABLE				(0x0 << 11)
#define PAD_CTL_ODE_ENABLE				(0x1 << 11)

#define PAD_CTL_ODT
#define PAD_CTL_SPEED_TBD				(0x0 << 6)
#define PAD_CTL_SPEED_LOW				(0x1 << 6)
#define PAD_CTL_SPEED_MEDIUM			(0x2 << 6)
#define PAD_CTL_SPEED_MAX				(0x3 << 6)

#define PAD_CTL_DSE_DISABLE				(0x0 << 3)
#define PAD_CTL_DSE_240_OHM				(0x1 << 3)
#define PAD_CTL_DSE_120_OHM				(0x2 << 3)
#define PAD_CTL_DSE_80_OHM				(0x3 << 3)
#define PAD_CTL_DSE_60_OHM				(0x4 << 3)
#define PAD_CTL_DSE_48_OHM				(0x5 << 3)
#define PAD_CTL_DSE_40_OHM				(0x6 << 3)
#define PAD_CTL_DSE_34_OHM				(0x7 << 3)

#define PAD_CTL_SRE_SLOW				(0x0 << 0)
#define PAD_CTL_SRE_FAST				(0x1 << 0)

void pinmux_set_swmux(int pin, int mux_config);
void pinmux_set_padcfg(int pin, int pad_config);
void pinmux_set_input(int pin, int input_config);


#endif	// SDK_LIB_INCLUDE_ARM_MX6X_IOMUX_H
