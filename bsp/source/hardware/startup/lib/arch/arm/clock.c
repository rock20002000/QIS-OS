#include <platform/arch/arm/include/timer/clock.h>
#include <platform/arch/arm/include/mx6q/crm_regs.h>
#include <platform/arch/arm/include/mx6q/mx6x.h>
#include "io.h"
#include <libc/include/stdio.h>


struct mxc_ccm_reg *imx_ccm = (struct mxc_ccm_reg *)MX6X_CCM_BASE;

static u32 decode_pll(enum pll_clocks pll, u32 infreq)
{
	u32 div;

	switch (pll) {
	case PLL_SYS:
		div = readl(&imx_ccm->analog_pll_sys);
		div &= BM_ANADIG_PLL_SYS_DIV_SELECT;

		return infreq * (div >> 1);
	case PLL_BUS:
		div = readl(&imx_ccm->analog_pll_528);
		div &= BM_ANADIG_PLL_528_DIV_SELECT;

		return infreq * (20 + (div << 1));
	case PLL_USBOTG:
		div = readl(&imx_ccm->analog_usb1_pll_480_ctrl);
		div &= BM_ANADIG_USB1_PLL_480_CTRL_DIV_SELECT;

		return infreq * (20 + (div << 1));
	case PLL_ENET:
		div = readl(&imx_ccm->analog_pll_enet);
		div &= BM_ANADIG_PLL_ENET_DIV_SELECT;

		return (div == 3 ? 125000000 : 25000000 * (div << 1));
	default:
		return 0;
	}
	/* NOTREACHED */
}

static u32 get_mcu_main_clk(void)
{
	u32 reg, freq;

	reg = readl(&imx_ccm->cacrr);
	reg &= MXC_CCM_CACRR_ARM_PODF_MASK;
	reg >>= MXC_CCM_CACRR_ARM_PODF_OFFSET;
	freq = decode_pll(PLL_SYS, MXC_HCLK);

	return freq / (reg + 1);
}

u32 get_periph_clk(void)
{
	u32 reg, freq = 0;

	reg = readl(&imx_ccm->cbcdr);
	if (reg & MXC_CCM_CBCDR_PERIPH_CLK_SEL) {
		reg = readl(&imx_ccm->cbcmr);
		reg &= MXC_CCM_CBCMR_PERIPH_CLK2_SEL_MASK;
		reg >>= MXC_CCM_CBCMR_PERIPH_CLK2_SEL_OFFSET;

		switch (reg) {
		case 0:
			freq = decode_pll(PLL_USBOTG, MXC_HCLK);
			break;
		case 1:
		case 2:
			freq = MXC_HCLK;
			break;
		default:
			break;
		}
	} else {
		reg = readl(&imx_ccm->cbcmr);
		reg &= MXC_CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK;
		reg >>= MXC_CCM_CBCMR_PRE_PERIPH_CLK_SEL_OFFSET;

		switch (reg) {
		case 0:
			freq = decode_pll(PLL_BUS, MXC_HCLK);
			break;
		case 1:
			freq = PLL2_PFD2_FREQ;
			break;
		case 2:
			freq = PLL2_PFD0_FREQ;
			break;
		case 3:
			freq = PLL2_PFD2_DIV_FREQ;
			break;
		default:
			break;
		}
	}

	return freq;
}

u32 get_ahb_clk(void)
{
	struct mxc_ccm_reg *imx_ccm = (struct mxc_ccm_reg *)MX6X_CCM_BASE;
	u32 reg, ahb_podf;

	reg = readl(&imx_ccm->cbcdr);
	reg &= MXC_CCM_CBCDR_AHB_PODF_MASK;
	ahb_podf = reg >> MXC_CCM_CBCDR_AHB_PODF_OFFSET;

	return get_periph_clk() / (ahb_podf + 1);
}

static u32 get_ipg_clk(void)
{
	u32 reg, ipg_podf;

	reg = readl(&imx_ccm->cbcdr);
	reg &= MXC_CCM_CBCDR_IPG_PODF_MASK;
	ipg_podf = reg >> MXC_CCM_CBCDR_IPG_PODF_OFFSET;

	return get_ahb_clk() / (ipg_podf + 1);
}

static u32 get_ipg_per_clk(void)
{
	u32 reg, perclk_podf;

	reg = readl(&imx_ccm->cscmr1);
	perclk_podf = reg & MXC_CCM_CSCMR1_PERCLK_PODF_MASK;

	return get_ipg_clk() / (perclk_podf + 1);
}

static u32 get_uart_clk(void)
{
	u32 reg, uart_podf;
	u32 freq = PLL3_80M;
	reg = readl(&imx_ccm->cscdr1);
#ifdef CONFIG_MX6SL
	if (reg & MXC_CCM_CSCDR1_UART_CLK_SEL)
		freq = MXC_HCLK;
#endif
	reg &= MXC_CCM_CSCDR1_UART_CLK_PODF_MASK;
	uart_podf = reg >> MXC_CCM_CSCDR1_UART_CLK_PODF_OFFSET;

	return freq / (uart_podf + 1);
}

unsigned int mxc_get_clock(enum mxc_clock clk)
{
	switch (clk) {
	case MXC_ARM_CLK:
		return get_mcu_main_clk();
	case MXC_PER_CLK:
		return get_periph_clk();
	case MXC_AHB_CLK:
		return get_ahb_clk();
	case MXC_IPG_CLK:
		return get_ipg_clk();
	case MXC_UART_CLK:
		return get_uart_clk();
    case MXC_IPG_PERCLK:
		return get_ipg_per_clk();    

	default:
		break;
	}

	return -1;
}


void init_clocks(void)
{
	/* Restore the default values in the Gate registers */
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR0), MX6X_CCM_CCGR0_RESET);
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR1), MX6X_CCM_CCGR1_RESET);
#if defined (VARIANT_enableTZASC)
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR2), 0xFFFFFFFF);	// Enable Trust Zone Address Space Controller clocks
#else
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR2), MX6X_CCM_CCGR2_RESET);
#endif
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR3), MX6X_CCM_CCGR3_RESET);
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR4), MX6X_CCM_CCGR4_RESET);
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR5), MX6X_CCM_CCGR5_RESET);
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR6), MX6X_CCM_CCGR6_RESET);
	out32((MX6X_CCM_BASE + MX6X_CCM_CCGR7), MX6X_CCM_CCGR7_RESET);

	/* TODO: No documentation about this? */
	unsigned int val = in32(MX6X_ANATOP_BASE + MX6X_ANADIG_REG_CORE);
	val &= ~ANADIG_REG_CORE_REG2_TRG_MASK;
	val |= ((0x14 << 18) & ANADIG_REG_CORE_REG2_TRG_MASK);
	out32(MX6X_ANATOP_BASE + MX6X_ANADIG_REG_CORE, val);

	/* Gate/ungate PFDs to ensure  proper reset (imX6DQ Erratum ERR006282) */
	out32(MX6X_ANATOP_BASE + MX6X_ANATOP_PFD_480_SET, MX6X_ANATOP_PFD_ALL_CLOCK_GATES);
	out32(MX6X_ANATOP_BASE + MX6X_ANATOP_PFD_528_SET, MX6X_ANATOP_PFD_ALL_CLOCK_GATES);
	out32(MX6X_ANATOP_BASE + MX6X_ANATOP_PFD_480_CLR, MX6X_ANATOP_PFD_ALL_CLOCK_GATES);
	out32(MX6X_ANATOP_BASE + MX6X_ANATOP_PFD_528_CLR, MX6X_ANATOP_PFD_ALL_CLOCK_GATES);
}

