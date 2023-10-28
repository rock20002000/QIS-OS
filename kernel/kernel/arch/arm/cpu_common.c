#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <utils/include/print_macros.h>
#include <platform/arch/arm/include/intr/gic.h>

//#include "apic.h"
#include "interrupt.h"
#include "kernel.h"
#include "trap.h"
#include "cpu_common.h"
#include "cpu/per_cpu.h"

static struct cpuvar cpuvars[NUM_CPUS_MAX] = {0};
//static
vaddr_t kernel_stack_syscall_global[NUM_CPUS_MAX] = {0};
vaddr_t kernel_stack_interrupt_global[NUM_CPUS_MAX] = {0};

extern vaddr_t g_gptvaddr;

//	static inline
struct cpuvar *get_cpuvar(void) { return &cpuvars[mp_self()]; }

//int ARM_MRS_MP_SELF()
//{
//	uint value;
//	__asm__ __volatile__("mrc p15,0, %0,c0,c0,5" : "=r"(value));
//	return value;
//}


//
//void put_mx1(int data)
//{
//	while (!(in32(0x021e8000 + 0x94) & (1<<13)))
//		;
//	out32(0x021e8000 + 0x40, (unsigned)data);
//}
//void printf_t(char* pszTmp){
//	while((*pszTmp)!='\0'){
//		put_mx1(*pszTmp);	//	x64_screen_printchar(*pszTmp);
//		pszTmp++;
//	}
//	return;
//}
//
//unsigned long handle_swi_exception(iframe_t* iframe){	//	long n, long a1, long a2, long a3, long a4, long a5){
//	 long n 	= iframe->r0;
//	 long a1	= iframe->r1;
//	 long a2 	= iframe->r2;
//	 long a3 	= iframe->r3;
//	 long a4 	= iframe->r4;
//	 long a5 	= iframe->r5;
//
//	//printf_t("Entry SWI entry\n");
//	if(n==1){
//		put_mx1(a1);
//	}
//	//printf_t("Leave SWI entry\n");
//	return 0;
//}

//GIC_Type * get_gic_base(void)
//{
//	GIC_Type *dst;
//
//	__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst));
//	printf("GIC Base:0x%p\n", dst);
//	return dst;
//}

//extern vaddr_t g_gicvaddr;
//unsigned long handle_interrupt_withIrqNo(uint32_t giccIar){	//	iframe_t* iframe, uint32_t giccIar){
//		int nr;
//		//printf_t("Entry interrupt handler\n");
//		GIC_V3_Type *gic = g_gicvaddr;	//	get_gic_base();
//		// The processor reads GICC_IAR to obtain the interrupt ID of the
//		// signaled interrupt. This read acts as an acknowledge for the interrupt
//		//	nr = gic->C_IAR;
//		nr = giccIar & 0x3FFUL;
//		if(nr!=87)printf("irq %d is happened\r\n", nr);
//
//		//	irq_table[nr].irq_handler(nr, irq_table[nr].param);
//
//		// write GICC_EOIR inform the CPU interface that it has completed
//		// the processing of the specified interrupt
//		gic->C_EOIR = nr;
//		//printf_t("Leave interrupt handler\n");
//		return 1;
//}
//
//unsigned long handle_interrupt(){
//	printf_t("Entry interrupt handler\n");
//	printf_t("Leave interrupt handler\n");
//	return 0;
//}
//

unsigned long handle_reset_exception(){
	printf("Entry handle_reset_exception\n");
	PANIC("RESET\n");
	printf("Leave handle_reset_exception\n");
	return 0;
}

unsigned long handle_reserve_exception(){
	printf("Entry handle_reserve_exception\n");
	PANIC("Reserve exception\n");
	printf("Leave handle_reserve_exception\n");
	return 0;
}


//	inline
void ack_irq(int iIrqVec){

	if (g_gptvaddr != NULL_VADDR) {
		GPT_V2_Type* pGpt = (GPT_V2_Type*)g_gptvaddr;
		pGpt->SR |= 1;
	}
}

//void GPT1_COMP1_handle_irq(void)
//{
//	static int on = 1;
//	printf_t("GPT1 comp0 interrupt happened\r\n");
//	// bit0: OF1 Output Compare 1 Flag
//	// write 1 clear it
//	GPT1->SR |= 1;
//	// read GPIO5_DR to get GPIO5_IO01 status
//	if(on) {
//		// led off, set GPIO5_DR to configure GPIO5_IO03 output 1
//		GPIO5->DR |= (1<<3); //led off
//		on = 0;
//	} else {
//		// led on, set GPIO5_DR to configure GPIO5_IO03 output 0
//		GPIO5->DR &= ~(1<<3); //led on
//		on = 1;
//	}
//}

//
//void system_init()
//{
//      //////////////////////////////////////////
//
//    init_aips();
//    arch_cpu_init();
//    init_clocks();
//    init_pinmux();
//
//    /* Init serial interface */
//	init_serial_mx6q();
//
//    system_init_irq_table();
//
//    init_intrinfo();
//
//    gic_init();
//
//    gpt_init(GPT1, OUT_COMP1, 1000000);// set 1s
//
//    request_irq(GPT1_IRQn, (irq_handler_t)GPT1_COMP1_handle_irq, NULL);
//
//    gic_enable_irq(GPT1_IRQn);
//	gpt_enable_interrupt(GPT1, IR_OF1IE, 1);
//	gpt_run(GPT1, 1);
//
//	ser_putstr("\n gpt1 start!! \n");
//
//    while(1);
//
//}

//static sys_irq_handle_t irq_table[160];
//
//static void default_irq_handler2 (uint32_t nr, void *param) {
//	printf("irq%d have not set irq handler, use default stead\n", nr);
//}
//
//extern int _vector_table;
//void system_init_irq_table(void)
//{
//	uint32_t i;
//
//	/* First set all handler to default */
//	for (i = 0; i < (sizeof(irq_table) / sizeof(irq_table[0]) - 1); i++) {
//	  request_irq((IRQn_Type)i, default_irq_handler2, NULL);
//	}
//
//	__set_VBAR((uint32_t)&_vector_table); 	/* �ж�������ƫ�ƣ�ƫ�Ƶ���ʼ��ַ */
//}

extern void	init_traps(void);
void cpu_common_setup(syspage_t* p_syspage) {
	arch_intr_init();
	init_traps();
}


uint32_t rand_r(unsigned *seed) {
	*seed = *seed * 2173565273 + 37623;
	return (unsigned)(*seed);	// / 65536) % 32768;
}

static unsigned	volatileSeed = 51263;

uint32_t rand(void) {
	volatileSeed += getTicks();
	return rand_r(&volatileSeed);
}

uint32_t get_random_long(void){
	return rand();
}

uint32_t get_random_int(void){
	return rand();
}
