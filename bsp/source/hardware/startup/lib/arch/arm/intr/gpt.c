#include <platform/arch/arm/include/timer/gpt.h>
#include <platform/arch/arm/include/mx6q/imx-regs.h>
#include <platform/arch/arm/include/timer/clock.h>
#include <boot/include/bootinfo.h>
#include "io.h"
#include "div64.h"

#define CLK_MHZ	(66)

/* assume use ipc clk which is 66MHz, 1us against to 66 count */
#define USEC_TO_COUNT(us) (us * CLK_MHZ - 1)

/* General purpose timers registers */
struct mxc_gpt {
	unsigned int control;
	unsigned int prescaler;
	unsigned int status;
	unsigned int nouse[6];
	unsigned int counter;
};
#define AIPS1_OFF_BASE_ADDR         (ATZ1_BASE_ADDR + 0x80000)
// static struct mxc_gpt *cur_gpt = (struct mxc_gpt *)GPT1_BASE_ADDR;

/* General purpose timers bitfields */
#define GPTCR_SWR		(1 << 15)	/* Software reset */
#define GPTCR_FRR		(1 << 9)	/* Freerun / restart */
#define GPTCR_CLKSOURCE_32	(4 << 6)	/* Clock source */
#define GPTCR_TEN		1		/* Timer enable */

static size_t get_cycles(const struct systime_entry* p_systime)
{
	if (!p_systime)
		return 0LL;

	GPT_Type* gpt = (GPT_Type*)(p_systime->st_base);

	return gpt->CNT;
	// return gpt->OCR[p_systime->channel] - gpt->CNT;
}

void gpt_init(GPT_Type *base, enum gpt_comp_channel chan, struct systime_entry* p_systime)
{
	/* bit15 SWR, Software reset*/
	base->CR |= (1 << 15);
	/* Wait reset finished. */
	while((base->CR >> 15) & 0x1) {
	}
		  			 		  						  					  				 	   		  	  	 	  
	/*
	 *bit10: Enable 24 MHz clock input from crystal
	 *bit9: 0 restart mode, 1 free-run mode:set 0
	 *bit8-6: Clock Source select :001 Peripheral Clock (ipg_clk)
	 *bit5: GPT Stop Mode enable
	 *bit3: GPT Wait Mode enable.
	 *bit1: GPT Enable Mode
	 */	
	base->CR = (1 << 6) | (1 << 5) | (1 << 3) | (1 << 1);

	/*
	 *bit15-bit12:PRESCALER24M
	 *bit11-0:PRESCALER
	 */
	base->PR = 0;

	uint32_t us = 1000*1000 / p_systime->timer_freq;
	/* GPTx_OCR1  bit31-0: Compare Value
	 * When the counter value equals the COMP bit field value, 
	 * a compare event is generated on Output Compare Channel 1.
	 */
	base->OCR[chan] = USEC_TO_COUNT(us);

	p_systime->get_cycles = get_cycles;
	p_systime->code_size = (uint16_t)((vaddr_t)gpt_init - (vaddr_t)get_cycles);
	p_systime->cycles_per_sec = CLK_MHZ * 1000 * 1000;
	p_systime->channel = (uint16_t)chan;
	p_systime->st_size = sizeof(GPT_Type);
	p_systime->st_base = (addr_t)base;
}

void gpt_run(GPT_Type *base, int on)
{
	/* bit0: GPT Enable */
	if (on)
		base->CR |= (1 << 0);
	else
		base->CR &= ~(1 << 0);
}

/* 
 * bit5: Rollover Interrupt Enable
 * bit4: IF2 Input capture Interrupt Enable
 * bit3: IF1IE Input capture Interrupt Enable
 * bit2: OF3IE Output Compare 3 Interrupt Enable
 * bit1: OF2IE Output Compare 2 Interrupt Enable
 * bit0: OF1IE Output Compare 1 Interrupt Enable
 */
void gpt_enable_interrupt(GPT_Type *base, enum gpt_interrupt_bit bit, int on)
{
	if (on)
		base->IR |= (1 << bit);
	else
		base->IR &= ~(1 << bit);
}

void gpt_clear_int_status(GPT_Type *base, enum gpt_interrupt_bit bit)
{
		base->SR |= (1 << bit);
}

static inline unsigned long long tick_to_time(unsigned long long tick)
{
	tick *= CONFIG_SYS_HZ;
	do_div(tick, MXC_CLK32);

	return tick;
}

static inline unsigned long long us_to_tick(unsigned long long usec)
{
	usec = usec * MXC_CLK32 + 999999;
	do_div(usec, 1000000);

	return usec;
}

/*
extern syspage_t g_syspage;
int timer_init(void)
{
	int i;

	// setup GP Timer 1
	writel(GPTCR_SWR, &cur_gpt->control);

	// We have no udelay by now
	for (i = 0; i < 100; i++)
		writel(0, &cur_gpt->control);

	writel(0, &cur_gpt->prescaler); // 32Khz

	// Freerun Mode, PERCLK1 input
	i = readl(&cur_gpt->control);
	writel(i | GPTCR_CLKSOURCE_32 | GPTCR_TEN, &cur_gpt->control);

	g_syspage.gptimer.tbl = readl(&cur_gpt->counter);
	g_syspage.gptimer.tbu = 0;

	return 0;
}

unsigned long long get_ticks(void)
{
	unsigned long now = readl(&cur_gpt->counter); // current tick value

	// increment tbu if tbl has rolled over
	if (now < g_syspage.gptimer.tbl)
		g_syspage.gptimer.tbu++;
	g_syspage.gptimer.tbl = now;
	return (((unsigned long long)g_syspage.gptimer.tbu) << 32) | g_syspage.gptimer.tbl;
}

// delay x useconds AND preserve advance timstamp value
void __udelay(unsigned long usec)
{
	unsigned long long tmp;
	unsigned long tmo;

	tmo = us_to_tick(usec);
	tmp = get_ticks() + tmo;	// 	get current timestamp

	while (get_ticks() < tmp)	// 	loop till event
		 ;						//	NOP
}	*/

unsigned long long get_ticks(void)
{
	return 0LL;
}

unsigned long get_timer_masked(void)
{
	/*
	 * get_ticks() returns a long long (64 bit), it wraps in
	 * 2^64 / MXC_CLK32 = 2^64 / 2^15 = 2^49 ~ 5 * 10^14 (s) ~
	 * 5 * 10^9 days... and get_ticks() * CONFIG_SYS_HZ wraps in
	 * 5 * 10^6 days - long enough.
	 */
	return tick_to_time(get_ticks());
}

unsigned long get_timer(unsigned long base)
{
	return get_timer_masked() - base;
}


/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return MXC_CLK32;
}
