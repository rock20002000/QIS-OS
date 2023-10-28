#include <platform/arch/arm/include/mx6q/mx6x_iomux.h>
#include <platform/arch/arm/include/mx6q/mx6x.h>
#include <libc/include/stdio.h>
#include "debug_dev.h"


#define MX6Q_PAD_SETTINGS_UART (PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE | \
                				PAD_CTL_PUS_100K_PU | PAD_CTL_SPEED_MEDIUM | \
                                PAD_CTL_DSE_40_OHM | PAD_CTL_SRE_FAST | PAD_CTL_PUE_PULL)


static inline void set_mux_cfg(unsigned int pin, unsigned int value)
{
	out32((MX6X_IOMUXC_BASE + MX6X_IOMUX_SWMUX + (pin * 4)), value);
}

static inline void set_pad_cfg(unsigned int pin, unsigned int value)
{
	out32((MX6X_IOMUXC_BASE + MX6X_IOMUX_SWPAD + (pin * 4)), value);
}

static inline void set_pin_as_input(unsigned int pin, unsigned int value)
{
	out32((MX6X_IOMUXC_BASE + MX6X_IOMUX_SWINPUT + (pin * 4)), value);
}

#define MX6Q_PAD_SETTINGS_USDHC (PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PULL | PAD_CTL_PUS_47K_PU | \
                                PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80_OHM | PAD_CTL_SRE_FAST | \
                                PAD_CTL_HYS_ENABLE)

/*
 * The Sabre-Lite board has a 50kOhm pull-down resistor connected to SD slot 3's write protect signal (SD3_WP).
 * The pad connected to the SD3_WP signal is called SD3_DAT4, due to the high pull-down resistor we must connect
 * a low internal pull-up resistor to pad SD3_DAT4 to ensure that SD3_WP's voltages are interpreted by the i.MX6Q correctly.
 */
#define MX6Q_PAD_SETTINGS_USDHC3_WP (PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PULL | PAD_CTL_PUS_22K_PU | PAD_CTL_SPEED_LOW | \
                            PAD_CTL_DSE_80_OHM | PAD_CTL_HYS_ENABLE)

#define MX6Q_PAD_SETTINGS_ECSPI (PAD_CTL_SRE_FAST | PAD_CTL_SPEED_MEDIUM | PAD_CTL_DSE_40_OHM | PAD_CTL_HYS_ENABLE)



void init_pinmux()
{
    /*
     * UART
     */
#ifdef DIGI_BOARD
	/* UART4 RXD */
	set_mux_cfg(SWMUX_KEY_ROW0, MUX_CTL_MUX_MODE_ALT4);
	set_pad_cfg(SWMUX_KEY_ROW0, MX6Q_PAD_SETTINGS_UART);
	set_pin_as_input(SWINPUT_UART4_IPP_UART_RXD_MUX, 1);

	/* UART4 TXD */
	set_mux_cfg(SWMUX_CSI0_DAT12, MUX_CTL_MUX_MODE_ALT4);
	set_pad_cfg(SWMUX_CSI0_DAT12, MX6Q_PAD_SETTINGS_UART);
#else
	/* UART1 RXD */
	set_mux_cfg(SWMUX_SD3_DAT7, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD3_DAT7, MX6Q_PAD_SETTINGS_UART);
	
	/* UART1 TXD */
	set_mux_cfg(SWMUX_SD3_DAT6, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD3_DAT6, MX6Q_PAD_SETTINGS_UART);
	set_pin_as_input(SWINPUT_UART1_IPP_UART_RXD_MUX, 0x2);

	/* UART2 RXD */
	set_mux_cfg(SWMUX_EIM_D27, MUX_CTL_MUX_MODE_ALT4);
	set_pad_cfg(SWPAD_EIM_D27, MX6Q_PAD_SETTINGS_UART);
	set_pin_as_input(SWINPUT_UART2_IPP_UART_RXD_MUX, 0x1);
	
	/* UART2 TXD */
	set_mux_cfg(SWMUX_EIM_D26, MUX_CTL_MUX_MODE_ALT4);
	set_pad_cfg(SWPAD_EIM_D26, MX6Q_PAD_SETTINGS_UART);
#endif
	/*
     * USDHC interfaces 
     */

	/* SD3 CLK */
	set_mux_cfg(SWMUX_SD3_CLK, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD3_CLK, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 CMD */
	set_mux_cfg(SWMUX_SD3_CMD, MUX_CTL_MUX_MODE_ALT0 | MUX_CTL_SION);
	set_pad_cfg(SWPAD_SD3_CMD, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 DAT0 */
	set_mux_cfg(SWMUX_SD3_DAT0, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD3_DAT0, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 DAT1 */
	set_mux_cfg(SWMUX_SD3_DAT1, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD3_DAT1, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 DAT2 */
	set_mux_cfg(SWMUX_SD3_DAT2, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD3_DAT2, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 DAT3 */
	set_mux_cfg(SWMUX_SD3_DAT3, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD3_DAT3, MX6Q_PAD_SETTINGS_USDHC);

	/* SD3 Write Protect - configure GPIO7[1] as an input */
	set_mux_cfg(SWMUX_SD3_DAT4, MUX_CTL_MUX_MODE_ALT5);
	set_pad_cfg(SWPAD_SD3_DAT4, MX6Q_PAD_SETTINGS_USDHC3_WP);
	out32(MX6X_GPIO7_BASE + MX6X_GPIO_GDIR, in32(MX6X_GPIO7_BASE + MX6X_GPIO_GDIR) & ~(1<<1));

	/* SD3 Card Detect - configure GPIO7[0] as an input */
	set_mux_cfg(SWMUX_SD3_DAT5, MUX_CTL_MUX_MODE_ALT5);
	out32(MX6X_GPIO7_BASE + MX6X_GPIO_GDIR, in32(MX6X_GPIO7_BASE + MX6X_GPIO_GDIR) & ~(1<<0));

	/* SD4 CLK */
	set_mux_cfg(SWMUX_SD4_CLK, MUX_CTL_MUX_MODE_ALT0);
	set_pad_cfg(SWPAD_SD4_CLK, MX6Q_PAD_SETTINGS_USDHC);

	/* SD4 CMD */
	set_mux_cfg(SWMUX_SD4_CMD, MUX_CTL_MUX_MODE_ALT0 | MUX_CTL_SION);
	set_pad_cfg(SWPAD_SD4_CMD, MX6Q_PAD_SETTINGS_USDHC);

	/* SD4 DAT0 */
	set_mux_cfg(SWMUX_SD4_DAT0, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD4_DAT0, MX6Q_PAD_SETTINGS_USDHC);

	/* SD4 DAT1 */
	set_mux_cfg(SWMUX_SD4_DAT1, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD4_DAT1, MX6Q_PAD_SETTINGS_USDHC);

	/* SD4 DAT2 */
	set_mux_cfg(SWMUX_SD4_DAT2, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD4_DAT2, MX6Q_PAD_SETTINGS_USDHC);

	/* SD4 DAT3 */
	set_mux_cfg(SWMUX_SD4_DAT3, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_SD4_DAT3, MX6Q_PAD_SETTINGS_USDHC);

 	/* SD4 Card Detect - configure GPIO2[6] as an input */
	set_mux_cfg(SWMUX_NANDF_D6, MUX_CTL_MUX_MODE_ALT5);
	out32(MX6X_GPIO2_BASE + MX6X_GPIO_GDIR, in32(MX6X_GPIO2_BASE + MX6X_GPIO_GDIR) & ~(1<<6));

	/* SD4 Write Protect - configure GPIO2[7] as an input */
	set_mux_cfg(SWMUX_NANDF_D7, MUX_CTL_MUX_MODE_ALT5);
	out32(MX6X_GPIO2_BASE + MX6X_GPIO_GDIR, in32(MX6X_GPIO2_BASE + MX6X_GPIO_GDIR) & ~(1<<7));

	/*
	* SPI
	*/

	/* SPI SCLK */
	set_mux_cfg(SWMUX_EIM_D16, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_EIM_D16, MX6Q_PAD_SETTINGS_ECSPI);
	set_pin_as_input(SWINPUT_ECSPI1_IPP_CSPI_CLK, 0x0);

	/* SPI MISO */
	set_mux_cfg(SWMUX_EIM_D17, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_EIM_D17, MX6Q_PAD_SETTINGS_ECSPI);
	set_pin_as_input(SWINPUT_ECSPI1_IPP_IND_MISO, 0x0);

	/* SPI MOSI */
	set_mux_cfg(SWMUX_EIM_D18, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_EIM_D18, MX6Q_PAD_SETTINGS_ECSPI);
	set_pin_as_input(SWINPUT_ECSPI1_IPP_IND_MOSI, 0x0);

	/* Select mux mode ALT1 for SS1 */
	set_mux_cfg(SWMUX_EIM_D19, MUX_CTL_MUX_MODE_ALT1);
	set_pad_cfg(SWPAD_EIM_D19, MX6Q_PAD_SETTINGS_ECSPI);
	set_pin_as_input(SWINPUT_ECSPI1_IPP_IND_SS_B_1, 0x0);
}


void init_aips()
{
    /*
     * Set all MPROTx to be non-bufferable, trusted for R/W,
     * not forced to user-mode.
     */
	out32(MX6X_AIPS1_CONFIG, 0x77777777);
	out32((MX6X_AIPS1_CONFIG + 0x04), 0x77777777);
	out32((MX6X_AIPS1_CONFIG + 0x40), 0x0);
	out32((MX6X_AIPS1_CONFIG + 0x44), 0x0);
	out32((MX6X_AIPS1_CONFIG + 0x48), 0x0);
	out32((MX6X_AIPS1_CONFIG + 0x4C), 0x0);
	out32((MX6X_AIPS1_CONFIG + 0x50), 0x0);

	out32(MX6X_AIPS2_CONFIG, 0x77777777);
	out32((MX6X_AIPS2_CONFIG + 0x04), 0x77777777);
	out32((MX6X_AIPS2_CONFIG + 0x40), 0x0);
	out32((MX6X_AIPS2_CONFIG + 0x44), 0x0);
	out32((MX6X_AIPS2_CONFIG + 0x48), 0x0);
	out32((MX6X_AIPS2_CONFIG + 0x4C), 0x0);
	out32((MX6X_AIPS2_CONFIG + 0x50), 0x0);
}
