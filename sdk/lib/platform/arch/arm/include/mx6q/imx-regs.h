#pragma once

#include "types.h"

#define AIPS1_ARB_BASE_ADDR             0x02000000
#define AIPS1_ARB_END_ADDR              0x020FFFFF
#define ATZ1_BASE_ADDR              AIPS1_ARB_BASE_ADDR
#define AIPS1_OFF_BASE_ADDR         (ATZ1_BASE_ADDR + 0x80000)
#define IOMUXC_BASE_ADDR            (AIPS1_OFF_BASE_ADDR + 0x60000)
#define AIPS1_ON_BASE_ADDR          (ATZ1_BASE_ADDR + 0x7C000)
#define AIPS1_BASE_ADDR             AIPS1_ON_BASE_ADDR



#define AIPS2_ARB_BASE_ADDR             0x02100000
#define ATZ2_BASE_ADDR              AIPS2_ARB_BASE_ADDR
#define AIPS2_ON_BASE_ADDR          (ATZ2_BASE_ADDR + 0x7C000)
#define AIPS2_BASE_ADDR             AIPS2_ON_BASE_ADDR

#define ANATOP_BASE_ADDR            (AIPS1_OFF_BASE_ADDR + 0x48000)

#define WDOG1_BASE_ADDR             (AIPS1_OFF_BASE_ADDR + 0x3C000)
#define WDOG2_BASE_ADDR             (AIPS1_OFF_BASE_ADDR + 0x40000)

#define GPT1_BASE_ADDR              (AIPS1_OFF_BASE_ADDR + 0x18000)

#define WDOG1_BASE_ADDR             (AIPS1_OFF_BASE_ADDR + 0x3C000)

//#define CCM_BASE_ADDR               (AIPS1_OFF_BASE_ADDR + 0x44000)

/* Watchdog Timer (WDOG) registers */
struct wdog_regs {
	u16 wcr;	/* Control */
	u16 wsr;	/* Service */
	u16 wrsr;	/* Reset Status */
	u16 wicr;	/* Interrupt Control */
	u16 wmcr;	/* Misc Control */
};

#define WCR_WDZST	0x01
#define WCR_WDBG	0x02
#define WCR_WDE		0x04	/* WDOG enable */
#define WCR_WDT		0x08
#define WCR_WDW		0x80
#define SET_WCR_WT(x)	(x << 8)


struct aipstz_regs {
	u32	mprot0;
	u32	mprot1;
	u32	rsvd[0xe];
	u32	opacr0;
	u32	opacr1;
	u32	opacr2;
	u32	opacr3;
	u32	opacr4;
};

struct anatop_regs {
	u32	pll_sys;		/* 0x000 */
	u32	pll_sys_set;		/* 0x004 */
	u32	pll_sys_clr;		/* 0x008 */
	u32	pll_sys_tog;		/* 0x00c */
	u32	usb1_pll_480_ctrl;	/* 0x010 */
	u32	usb1_pll_480_ctrl_set;	/* 0x014 */
	u32	usb1_pll_480_ctrl_clr;	/* 0x018 */
	u32	usb1_pll_480_ctrl_tog;	/* 0x01c */
	u32	usb2_pll_480_ctrl;	/* 0x020 */
	u32	usb2_pll_480_ctrl_set;	/* 0x024 */
	u32	usb2_pll_480_ctrl_clr;	/* 0x028 */
	u32	usb2_pll_480_ctrl_tog;	/* 0x02c */
	u32	pll_528;		/* 0x030 */
	u32	pll_528_set;		/* 0x034 */
	u32	pll_528_clr;		/* 0x038 */
	u32	pll_528_tog;		/* 0x03c */
	u32	pll_528_ss;		/* 0x040 */
	u32	rsvd0[3];
	u32	pll_528_num;		/* 0x050 */
	u32	rsvd1[3];
	u32	pll_528_denom;		/* 0x060 */
	u32	rsvd2[3];
	u32	pll_audio;		/* 0x070 */
	u32	pll_audio_set;		/* 0x074 */
	u32	pll_audio_clr;		/* 0x078 */
	u32	pll_audio_tog;		/* 0x07c */
	u32	pll_audio_num;		/* 0x080 */
	u32	rsvd3[3];
	u32	pll_audio_denom;	/* 0x090 */
	u32	rsvd4[3];
	u32	pll_video;		/* 0x0a0 */
	u32	pll_video_set;		/* 0x0a4 */
	u32	pll_video_clr;		/* 0x0a8 */
	u32	pll_video_tog;		/* 0x0ac */
	u32	pll_video_num;		/* 0x0b0 */
	u32	rsvd5[3];
	u32	pll_video_denom;	/* 0x0c0 */
	u32	rsvd6[3];
	u32	pll_mlb;		/* 0x0d0 */
	u32	pll_mlb_set;		/* 0x0d4 */
	u32	pll_mlb_clr;		/* 0x0d8 */
	u32	pll_mlb_tog;		/* 0x0dc */
	u32	pll_enet;		/* 0x0e0 */
	u32	pll_enet_set;		/* 0x0e4 */
	u32	pll_enet_clr;		/* 0x0e8 */
	u32	pll_enet_tog;		/* 0x0ec */
	u32	pfd_480;		/* 0x0f0 */
	u32	pfd_480_set;		/* 0x0f4 */
	u32	pfd_480_clr;		/* 0x0f8 */
	u32	pfd_480_tog;		/* 0x0fc */
	u32	pfd_528;		/* 0x100 */
	u32	pfd_528_set;		/* 0x104 */
	u32	pfd_528_clr;		/* 0x108 */
	u32	pfd_528_tog;		/* 0x10c */
	u32	reg_1p1;		/* 0x110 */
	u32	reg_1p1_set;		/* 0x114 */
	u32	reg_1p1_clr;		/* 0x118 */
	u32	reg_1p1_tog;		/* 0x11c */
	u32	reg_3p0;		/* 0x120 */
	u32	reg_3p0_set;		/* 0x124 */
	u32	reg_3p0_clr;		/* 0x128 */
	u32	reg_3p0_tog;		/* 0x12c */
	u32	reg_2p5;		/* 0x130 */
	u32	reg_2p5_set;		/* 0x134 */
	u32	reg_2p5_clr;		/* 0x138 */
	u32	reg_2p5_tog;		/* 0x13c */
	u32	reg_core;		/* 0x140 */
	u32	reg_core_set;		/* 0x144 */
	u32	reg_core_clr;		/* 0x148 */
	u32	reg_core_tog;		/* 0x14c */
	u32	ana_misc0;		/* 0x150 */
	u32	ana_misc0_set;		/* 0x154 */
	u32	ana_misc0_clr;		/* 0x158 */
	u32	ana_misc0_tog;		/* 0x15c */
	u32	ana_misc1;		/* 0x160 */
	u32	ana_misc1_set;		/* 0x164 */
	u32	ana_misc1_clr;		/* 0x168 */
	u32	ana_misc1_tog;		/* 0x16c */
	u32	ana_misc2;		/* 0x170 */
	u32	ana_misc2_set;		/* 0x174 */
	u32	ana_misc2_clr;		/* 0x178 */
	u32	ana_misc2_tog;		/* 0x17c */
	u32	tempsense0;		/* 0x180 */
	u32	tempsense0_set;		/* 0x184 */
	u32	tempsense0_clr;		/* 0x188 */
	u32	tempsense0_tog;		/* 0x18c */
	u32	tempsense1;		/* 0x190 */
	u32	tempsense1_set;		/* 0x194 */
	u32	tempsense1_clr;		/* 0x198 */
	u32	tempsense1_tog;		/* 0x19c */
	u32	usb1_vbus_detect;	/* 0x1a0 */
	u32	usb1_vbus_detect_set;	/* 0x1a4 */
	u32	usb1_vbus_detect_clr;	/* 0x1a8 */
	u32	usb1_vbus_detect_tog;	/* 0x1ac */
	u32	usb1_chrg_detect;	/* 0x1b0 */
	u32	usb1_chrg_detect_set;	/* 0x1b4 */
	u32	usb1_chrg_detect_clr;	/* 0x1b8 */
	u32	usb1_chrg_detect_tog;	/* 0x1bc */
	u32	usb1_vbus_det_stat;	/* 0x1c0 */
	u32	usb1_vbus_det_stat_set;	/* 0x1c4 */
	u32	usb1_vbus_det_stat_clr;	/* 0x1c8 */
	u32	usb1_vbus_det_stat_tog;	/* 0x1cc */
	u32	usb1_chrg_det_stat;	/* 0x1d0 */
	u32	usb1_chrg_det_stat_set;	/* 0x1d4 */
	u32	usb1_chrg_det_stat_clr;	/* 0x1d8 */
	u32	usb1_chrg_det_stat_tog;	/* 0x1dc */
	u32	usb1_loopback;		/* 0x1e0 */
	u32	usb1_loopback_set;	/* 0x1e4 */
	u32	usb1_loopback_clr;	/* 0x1e8 */
	u32	usb1_loopback_tog;	/* 0x1ec */
	u32	usb1_misc;		/* 0x1f0 */
	u32	usb1_misc_set;		/* 0x1f4 */
	u32	usb1_misc_clr;		/* 0x1f8 */
	u32	usb1_misc_tog;		/* 0x1fc */
	u32	usb2_vbus_detect;	/* 0x200 */
	u32	usb2_vbus_detect_set;	/* 0x204 */
	u32	usb2_vbus_detect_clr;	/* 0x208 */
	u32	usb2_vbus_detect_tog;	/* 0x20c */
	u32	usb2_chrg_detect;	/* 0x210 */
	u32	usb2_chrg_detect_set;	/* 0x214 */
	u32	usb2_chrg_detect_clr;	/* 0x218 */
	u32	usb2_chrg_detect_tog;	/* 0x21c */
	u32	usb2_vbus_det_stat;	/* 0x220 */
	u32	usb2_vbus_det_stat_set;	/* 0x224 */
	u32	usb2_vbus_det_stat_clr;	/* 0x228 */
	u32	usb2_vbus_det_stat_tog;	/* 0x22c */
	u32	usb2_chrg_det_stat;	/* 0x230 */
	u32	usb2_chrg_det_stat_set;	/* 0x234 */
	u32	usb2_chrg_det_stat_clr;	/* 0x238 */
	u32	usb2_chrg_det_stat_tog;	/* 0x23c */
	u32	usb2_loopback;		/* 0x240 */
	u32	usb2_loopback_set;	/* 0x244 */
	u32	usb2_loopback_clr;	/* 0x248 */
	u32	usb2_loopback_tog;	/* 0x24c */
	u32	usb2_misc;		/* 0x250 */
	u32	usb2_misc_set;		/* 0x254 */
	u32	usb2_misc_clr;		/* 0x258 */
	u32	usb2_misc_tog;		/* 0x25c */
	u32	digprog;		/* 0x260 */
	u32	reserved1[7];
	u32	digprog_sololite;	/* 0x280 */
};

#define ANATOP_PFD_480_PFD0_FRAC_SHIFT		0
#define ANATOP_PFD_480_PFD0_FRAC_MASK		(0x3f<<ANATOP_PFD_480_PFD0_FRAC_SHIFT)
#define ANATOP_PFD_480_PFD0_STABLE_SHIFT	6
#define ANATOP_PFD_480_PFD0_STABLE_MASK		(1<<ANATOP_PFD_480_PFD0_STABLE_SHIFT)
#define ANATOP_PFD_480_PFD0_CLKGATE_SHIFT	7
#define ANATOP_PFD_480_PFD0_CLKGATE_MASK	(1<<ANATOP_PFD_480_PFD0_CLKGATE_SHIFT)
#define ANATOP_PFD_480_PFD1_FRAC_SHIFT		8
#define ANATOP_PFD_480_PFD1_FRAC_MASK		(0x3f<<ANATOP_PFD_480_PFD1_FRAC_SHIFT)
#define ANATOP_PFD_480_PFD1_STABLE_SHIFT	14
#define ANATOP_PFD_480_PFD1_STABLE_MASK		(1<<ANATOP_PFD_480_PFD1_STABLE_SHIFT)
#define ANATOP_PFD_480_PFD1_CLKGATE_SHIFT	15
#define ANATOP_PFD_480_PFD1_CLKGATE_MASK	(0x3f<<ANATOP_PFD_480_PFD1_CLKGATE_SHIFT)
#define ANATOP_PFD_480_PFD2_FRAC_SHIFT		16
#define ANATOP_PFD_480_PFD2_FRAC_MASK		(1<<ANATOP_PFD_480_PFD2_FRAC_SHIFT)
#define ANATOP_PFD_480_PFD2_STABLE_SHIFT	22
#define ANATOP_PFD_480_PFD2_STABLE_MASK	(1<<ANATOP_PFD_480_PFD2_STABLE_SHIFT)
#define ANATOP_PFD_480_PFD2_CLKGATE_SHIFT	23
#define ANATOP_PFD_480_PFD2_CLKGATE_MASK	(0x3f<<ANATOP_PFD_480_PFD2_CLKGATE_SHIFT)
#define ANATOP_PFD_480_PFD3_FRAC_SHIFT		24
#define ANATOP_PFD_480_PFD3_FRAC_MASK		(1<<ANATOP_PFD_480_PFD3_FRAC_SHIFT)
#define ANATOP_PFD_480_PFD3_STABLE_SHIFT	30
#define ANATOP_PFD_480_PFD3_STABLE_MASK		(1<<ANATOP_PFD_480_PFD3_STABLE_SHIFT)
#define ANATOP_PFD_480_PFD3_CLKGATE_SHIFT	31

struct iomuxc_base_regs {
	u32     gpr[14];        /* 0x000 */
	u32     obsrv[5];       /* 0x038 */
	u32     swmux_ctl[197]; /* 0x04c */
	u32     swpad_ctl[250]; /* 0x360 */
	u32     swgrp[26];      /* 0x748 */
	u32     daisy[104];     /* 0x7b0..94c */
};