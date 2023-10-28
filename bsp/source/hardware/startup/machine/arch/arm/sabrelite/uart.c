#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <boot/include/bootinfo.h>
#include <platform/arch/arm/include/mx6q/MCIMX6Y2.h>
#include <platform/arch/arm/include/mx6q/ipl_mx6x.h>
#include <platform/include/dump.h>
#include "generic/mx1.h"
#include "generic/mx6x.h"
#include "debug_dev.h"

#define UART_IN(r)      in32(base + (r))
#define UART_OUT(r, v)  out32(base + (r), v)
#define UART_CLR(r, m)  UART_OUT(r, UART_IN(r) & ~(m))
#define UART_SET(r, m)  UART_OUT(r, UART_IN(r) | (m))

#ifndef UART_TRDY_RETRY
 #define UART_TRDY_RETRY    0
#endif

// #define DUMP_REGS   14

static void init_debug_serial(bsp_device_t* p_bsp_dev)
{
    uint32_t base = DEBUG_DEV_BASE;

    if (p_bsp_dev)
        base = p_bsp_dev->io_addr;
#ifdef DUMP_REGS
    uint32_t buf[DUMP_REGS], cs13 = in32(0x20E028C), kr0 = in32(0x20E01FC), rx4mux = in32(0x20e0938);
    for (int i = 0; i < DUMP_REGS; i++)
        buf[i] = UART_IN(MX1_UART_CR1 + i*4);
#endif
    /* Wait for UART to finish transmitting */
    while (!(UART_IN(MX1_UART_BIPR2) & MXC_UTS_TXEMPTY));

	/* Disable UART */
    UART_CLR(MX1_UART_CR1, MXC_UCR1_UARTEN);

	/* Set to default POR state */
    UART_OUT(MX1_UART_CR1, 0);
    UART_OUT(MX1_UART_CR2, 0);

    while (!(UART_IN(MX1_UART_CR2) & MXC_UCR2_SRST));

    UART_OUT(MX1_UART_CR3, 0x0704);
    UART_OUT(MX1_UART_CR4, 0x8000);
    UART_OUT(MX1_UART_FCR, 0x0801);
    UART_OUT(MX1_UART_ESC, 0x002B);
    UART_OUT(MX1_UART_TIM, 0x0000);
    UART_OUT(MX1_UART_BIR, 0x0000);
    UART_OUT(MX1_UART_BMR, 0x0000);
    UART_OUT(MX1_UART_BIPR1, 0x0000);
    UART_OUT(MX1_UART_BIPR2, 0x0000);

     /* Configure FIFOs */
	UART_OUT(MX1_UART_FCR,
        ((1 << MXC_UFCR_RXTL_SHIFT) |
        MXC_UFCR_RFDIV_2 | (2 << MXC_UFCR_TXTL_SHIFT)));

	/* Set to 8N1 */
    UART_CLR(MX1_UART_CR2, MXC_UCR2_PREN);
    UART_SET(MX1_UART_CR2, MXC_UCR2_WS);
    UART_CLR(MX1_UART_CR2, MXC_UCR2_STPB);

    /* Ignore RTS */
    UART_SET(MX1_UART_CR2, MXC_UCR2_IRTS);

    UART_SET(MX1_UART_CR4, MXC_UCR4_DREN);		//	0x9;

     /* Enable UART */
	UART_SET(MX1_UART_CR1, MXC_UCR1_UARTEN);

	/* Enable FIFOs */
    UART_SET(MX1_UART_CR2, MXC_UCR2_SRST | MXC_UCR2_RXEN | MXC_UCR2_TXEN);

    /* Clear status flags */
    UART_SET(MX1_UART_SR2, MXC_USR2_ADET  |
	                  MXC_USR2_IDLE  |
	                  MXC_USR2_IRINT |
	                  MXC_USR2_WAKE  |
	                  MXC_USR2_RTSF  |
	                  MXC_USR2_BRCD  |
	                  MXC_USR2_ORE   |
	                  MXC_USR2_RDR);

    /* Clear status flags */
   UART_SET(MX1_UART_SR1, MXC_USR1_PARITYERR |
	                  MXC_USR1_RTSD      |
	                  MXC_USR1_ESCF      |
	                  MXC_USR1_FRAMERR   |
	                  MXC_USR1_AIRINT    |
	                  MXC_USR1_AWAKE);

	UART_OUT(MX1_UART_BIR, 0xF);
	UART_OUT(MX1_UART_BMR, 0x15B);
#ifdef DUMP_REGS
    dump_words("UART4", base+MX1_UART_CR1, 14, NULL_VADDR);
    dump_words("KEY_ROW0", &kr0, 1, 0x20E01FC);
    dump_words("CSI0_DAT13", &cs13, 1, 0x20E028C);
    dump_words("UART4_RXD_MUX", &rx4mux, 1, 0x20e0938);
#endif
}

//We just use putchar function of serial to print debug info(Assume that the serial device has been initialized porperly by uboot)
void debug_putc(bsp_device_t* p_bsp_dev, int data)
{
    uint32_t base = DEBUG_DEV_BASE;	//	MX6X_UART2_BASE;
    // int retry = 0;
    int max_retry = -1;

    if (p_bsp_dev)
    {
        base = p_bsp_dev->io_addr;
        max_retry = p_bsp_dev->DEB_DEV_PARAM_RETRY;
    }
    if (max_retry < 0)
        max_retry = UART_TRDY_RETRY;

    while (!(UART_IN(MX1_UART_SR1) & MX1_USR1_TRDY))	//while (!(UART_IN(MX1_UART_SR1) & MX1_USR1_TRDY) && (!max_retry || retry++ <= max_retry))
        ;

    UART_OUT(MX1_UART_TXDATA, (unsigned)data);
}

void debug_init(bsp_device_t* p_bsp_dev)
{
    // serial_init();
    if (p_bsp_dev)
    {
        if (!p_bsp_dev->io_size)
        {
            p_bsp_dev->io_addr = DEBUG_DEV_BASE;	//	MX6X_UART2_BASE;
            p_bsp_dev->io_size = 0x4000;
        }
        if (!p_bsp_dev->num_intf)
        {
            strcpy(p_bsp_dev->name, "uart2");
            p_bsp_dev->num_intf = DBG_DEV_INTF_NUMS;
            p_bsp_dev->irq_num = DEBUG_DEV_IRQ;
            p_bsp_dev->interface[0] = debug_init;
            p_bsp_dev->interface[DBG_PUTC_INDEX] = debug_putc;
            p_bsp_dev->DEB_DEV_PARAM_RETRY = UART_TRDY_RETRY;
        }
        // init_debug_serial(p_bsp_dev);
    }
    else
        init_debug_serial(NULL);
}

// Redefine weak function printchar to enable kpritf
void printchar(__unused struct vprintf_context *ctx, char ch) 
{
    if ('\n' == ch)
    {
        debug_putc(NULL, '\r');
    }
    debug_putc(NULL, ch);
}
