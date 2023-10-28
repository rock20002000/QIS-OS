#include "interrupt.h"

#include "aether_resmgr.h"
#include "arch_task.h"
#include "cpu_common.h"
#include "debug.h"
#include "ker_syscalls.h"
#include "kernel.h"
#include "task.h"
#include <platform/arch/arm/include/intr/gic.h>

static isr_t irq_handlers[160] __attribute__((aligned(16)));
//static sys_irq_handle_t irq_table[160];

extern vaddr_t g_gicvaddr;
extern vaddr_t g_gptvaddr;

void gen_softintr() {
	__asm__ __volatile__("swi 0");
}



//static void default_irq_handler (uint32_t nr, void *param) {
//	printf("irq%d have not set irq handler, use default stead\n", nr);
//}
//
//void request_irq(IRQn_Type irq, irq_handler_t handler, void *param)
//{
//  irq_table[irq].irq_handler = handler;
//  irq_table[irq].param = param;
//}

static void default_irq_handler (iframe_t* frame) {
	printf("The irq has not set irq handler, use default stead\n");

	//	printf("irq%d have not set irq handler, use default stead\n", nr);
}

void arm_handle_interrupt(unsigned int giccIar, iframe_t *frame)
{
	int cpu_intr_base = 0;

	GIC_V2_Type *gic = (GIC_V2_Type*)g_gicvaddr;	//	get_gic_base();

	// The processor reads GICC_IAR to obtain the interrupt ID of the
	// signaled interrupt. This read acts as an acknowledge for the interrupt
	//	nr = gic->C_IAR;
	
	unsigned short iIrqVec = gic->iIrqVec;	//	giccIar & 0x3FFUL;
	//if(iIrqVec!=87)printf("irq %d is happened\r\n", iIrqVec);
	
	paddr_t pageTableAddress = arch_pte_backup();

	get_systime(&p_vdso_data->interrupt_time);
//	if(iIrqVec == 88 )	//!= TIMER_IRQ)	//	87)	//cpu_intr_base)
//    {
//		KDBG("Interrupt triggered ... vec:%d \n", iIrqVec);	//arch_disable_irq(iIrqVec);
//    }

	if( iIrqVec > cpu_intr_base ){
		iIrqVec -= cpu_intr_base;

		//	threads created with InterruptAttach()
		THREAD_t* pThreadWaitTmp = lookupInterruptWaitQueueLookupListsWithIrq(iIrqVec);
		while(pThreadWaitTmp!=NULL){

			///	Note the irq mask is run with the thread registered for the irq ///
	    	arch_disable_irq(iIrqVec);
	    	////////////////////////////////////////////////////////////////////////

			pThreadWaitTmp->state = THREAD_STATE_READY;	//	STATE_SIGWAITINFO;
			interrupt_wait_queue_lookup_lists_remove(iIrqVec, pThreadWaitTmp);
			thread_runqueue_remove(pThreadWaitTmp);

			pThreadWaitTmp->schedulePolicy 	= SCHEDULE_POLICY_NORMAL;
			pThreadWaitTmp->priority		= 255;
			thread_privilegequeue_push(0, pThreadWaitTmp);
			pThreadWaitTmp = lookupInterruptWaitQueueLookupListsWithIrq(iIrqVec);
		}

		//	events queued by InterruptAttachEvent()
		int iLengthTmp = getLookupInterruptEventLookupListsLength(iIrqVec);
		struct sigevent * pSigEventTmp = NULL;
		for(int i=0; i<iLengthTmp; i++){
			pSigEventTmp = lookupInterruptEventLookupListsWithIrqIndex(iIrqVec, i);
			enqueueEventPulse(pSigEventTmp);
		}
		/*	struct sigevent * pSigEventTmp = lookupInterruptEventLookupListsWithIrq(iIrqVec);
		while(pSigEventTmp!=NULL)
		{
			enqueueEventPulse(pSigEventTmp);
			interrupt_event_lookup_lists_remove(iIrqVec, pSigEventTmp);
			pSigEventTmp = lookupInterruptEventLookupListsWithIrq(iIrqVec);
		}	*/

		iIrqVec += cpu_intr_base;
	}


//	if(iIrqVec==87){
//		if(g_gptvaddr!=NULL){
//				GPT_V3_Type* pGpt = (GPT_V3_Type*)g_gptvaddr;
//				pGpt->SR |= 1;
//			}
//	}else if(iIrqVec==59){
//		if (in32(g_syspage_ptr->debug_dev.io_addr + 0x94) & (1<<8))
//			out32(g_syspage_ptr->debug_dev.io_addr + 0x94, (1<<8));
//	}
	gic->C_EOIR = iIrqVec;
	gic->iEOIR	= iIrqVec;
	//gic->C_EOIR = 87;	//iIrqVec;



	arch_pte_restore(pageTableAddress);

    if(irq_handlers[iIrqVec] != NULL)
    {
        irq_handlers[iIrqVec](frame);
    }

	//	irq_table[nr].irq_handler(nr, irq_table[nr].param);

	// write GICC_EOIR inform the CPU interface that it has completed
	// the processing of the specified interrupt
	//	gic->C_EOIR = iIrqVec;


}



void init_irq_handler(void) {
	uint32_t i;

	/* First set all handler to default */
	for (i = 0; i < (sizeof(irq_handlers) / sizeof(irq_handlers[0]) - 1); i++) {
		register_interrupt_handler(i, default_irq_handler);	//	request_irq((IRQn_Type)i, default_irq_handler);	//default_irq_handler, NULL);
	}
}

void register_interrupt_handler(uint8_t vec, isr_t handler) {
	irq_handlers[vec] = handler;
	  //irq_table[irq].param = param;
}

long arm_handle_syscall(iframe_t* frame){//, long a1, long a2, long a3, long a4, long a5) {
	long ret = handle_syscall(frame);//, a1, a2, a3, a4, a5);
    //	long ret = handle_syscall(n, a1, a2, a3, a4, a5);	//

    return ret;
}



/* enable IRQ interrupts */
void arch_enable_irq (unsigned irq)
{
	GIC_V2_Type *gic = (GIC_V2_Type*)g_gicvaddr;	//	get_gic_base();

	//	printf("\n gic:0x%p\n", gic);

	/* The GICD_ISENABLERs provide a Set-enable bit for each interrupt supported by the GIC.
	 * Writing 1 to a Set-enable bit enables forwarding of the corresponding interrupt from the
	 * Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
	 */
	gic->D_ISENABLER[irq >> 5] |= (uint32_t)(1UL << (irq & 0x1FUL));
	gic->D_ICENABLER[irq >> 5] &= ~(uint32_t)(1UL << (irq & 0x1FUL));

}

void arch_enable_interrupt ()
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}


/*
 * disable IRQ/FIQ interrupts
 * returns true if interrupts had been enabled before we disabled them
 */
void arch_disable_irq (unsigned irq)
{
	GIC_V2_Type *gic = (GIC_V2_Type*)g_gicvaddr;	//	get_gic_base();

	// The GICD_ICENABLERs provide a Clear-enable bit for each interrupt supported by the
	// GIC. Writing 1 to a Clear-enable bit disables forwarding of the corresponding interrupt from
    // the Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
	gic->D_ICENABLER[irq >> 5] = (uint32_t)(1UL << (irq & 0x1FUL));
}

void arch_disable_interrupt ()
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return;	// (old & 0x80) == 0;
}

void printException(unsigned int cpsr, char *str)
{
	printf("Exception! cpsr is 0x%x\r\n", cpsr);
	printf("%s\r\n", str);
}

void printSWIVal(unsigned int *pSWI)
{
	printf("SWI val = 0x%x\r\n", *pSWI & ~0xff000000);
}

/* 
 void enable_interrupt(uint32_t irq_id, uint32_t cpu_id, uint32_t priority)
 {
     gic_set_irq_priority(irq_id, priority);
     gic_set_irq_security(irq_id, false);    // set IRQ as non-secure
     gic_set_cpu_target(irq_id, cpu_id, true);
     gic_enable_irq(irq_id, true);
 }
 
 void disable_interrupt(uint32_t irq_id, uint32_t cpu_id)
 {
     gic_enable_irq(irq_id, false);
     gic_set_cpu_target(irq_id, cpu_id, false);
 }

*/
