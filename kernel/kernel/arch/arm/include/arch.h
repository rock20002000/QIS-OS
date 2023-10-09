#ifndef __ARCH_H__
#define __ARCH_H__

#include <types.h>
#include <boot/include/bootinfo.h>

//#include "cpu/per_cpu.h"

#include "arch_mm.h"
#include "locks.h"

#include "interrupt.h"


// Interrupt & exception id
#define DIVIDE_ERR_IRQ_NUM 0  // IA32-3A.pdf p210
#define NMI_IRQ_NUM 2

#define IRQ_MAX 256

#define VECTOR_IRQ_BASE					0

#define TIMER_IRQ						87
#define KEYBOARD_IRQ 					0
#define CONSOLE_IRQ 					59
#define SPURIOUS_IRQ 					0
#define MOUSE_IRQ 						0
#define IDE_IRQ 						0
#define ERROR_IRQ 						0

#define DOUBLEERR_IRQ_KERNEL			(VECTOR_IRQ_BASE)
#define TIMER_IRQ_KERNEL				(VECTOR_IRQ_BASE+87)
#define KEYBOARD_IRQ_KERNEL				(VECTOR_IRQ_BASE+0)
#define CONSOLE_IRQ_KERNEL 				(VECTOR_IRQ_BASE+59)
#define SPURIOUS_IRQ_KERNEL 			(VECTOR_IRQ_BASE+0)
#define MOUSE_IRQ_KERNEL 				(VECTOR_IRQ_BASE+0)
#define IDE_IRQ_KERNEL 					(VECTOR_IRQ_BASE+0)
#define ERROR_IRQ_KERNEL 				(VECTOR_IRQ_BASE+0)



struct arch_thread {
    vaddr_t syscall_stack;
    vaddr_t interrupt_stack;
    //vaddr_t stack;				//	The stack is recorded as regs->sp
    /// The level-0 page table.
    //uint64_t *page_table;
    /// The user's page table paddr.
    //paddr_t ttbr0;

  //uintmax_t rsp;
  //uintmax_t interrupt_stack;
  //uintmax_t syscall_stack;
  //int fInitDone;
};

struct arch_proc {
  //uintmax_t gsbase;
  //uintmax_t fsbase;
  struct arch_mmu mmu;
};

/// CPU-local variables. Accessible through GS segment in kernel mode.
struct arch_cpuvar {
	uint32_t esp3;
	uint32_t esp2;
	uint32_t esp1;
	uint32_t esp0;
  //uintmax_t rsp0;
  // Temporarily used to switch the stack at the beginning of the syscall
  // handler.
  //uintmax_t rsp3;
} __attribute__((packed));


//	MRC p15,0,<Rt>,c0,c0,5
#define ARM_MRS_MP_SELF()                                                         \
    ({                                                                         \
        uint value;                                                        \
        __asm__ __volatile__("mrc p15,0, %0,c0,c0,5" : "=r"(value));                   \
        value;                                                                 \
    })


//#define ARM_MSR_TTBR0(uint32_t ttbr0)                                                         \
    ({                                                                         \
        __asm__ __volatile__("mcr p15, 0, %0, c2, c0, 0" : "r"(ttbr0));                   \
        value;                                                                 \
    })


//	int ARM_MRS_MP_SELF();

static inline int mp_self(void) {
    return (ARM_MRS_MP_SELF() & 0xff);
}


struct cpuvar;
#define NUM_CPUS_MAX	256
//static inline
struct cpuvar *get_cpuvar(void);
//extern struct cpuvar cpuvars[];
//static inline struct cpuvar *get_cpuvar(void) { return &cpuvars[mp_self()]; }	//return NULL; }
  
/// The current task of the current CPU (`struct task *`).
#define CURRENT_THREAD  (get_cpuvar()->current_task)
/// The idle task of the current CPU (`struct task *`).
#define IDLE_THREAD     (&get_cpuvar()->idle_task)
///
//	#define CURRENT_PGDIR   (&get_cpuvar()->cur_task_pgdir)





//
//  SYSCALL/SYSRET
//

//
//  Inline assembly.
//

/*
 * Read CPUID register
 */
static inline uint32_t arm_cpuid()
{
	uint32_t	id;
	__asm__ __volatile__(
		"mrc p15, 0, %0, c0, c0, 0"
		: "=r" (id)
		:
	);
	return id;
}


/*
 * Read MMU cache type register
 */
static inline uint32_t arm_mmu_cache_type()
{
	uint32_t	val;
	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 1" : "=r" (val));
	return val;
}

/*
 * Read the System Control Register
 */
static inline uint32_t arm_sctlr_get()
{
	uint32_t	val;
	__asm__ __volatile__("mrc p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}

/*
 * Write the System Control Register
 */
static inline void arm_sctlr_set(uint32_t sctlr)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c1, c0, 0" : : "r" (sctlr));
}

/*
 * Read the Memory Model Feature Register 2
 */
static inline uint32_t arm_mmfr2_get()
{
	uint32_t	val;
	__asm__ __volatile__("mrc p15, 0, %0, c0, c1, 6" : "=r" (val));
	return val;
}

/*
 * Write the Memory Model Feature Register 2
 */
static inline void arm_mmfr2_set(uint32_t mmfr2)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c0, c1, 6" : : "r" (mmfr2));
}

/*
 * Get TTBR0 register
 */
static inline paddr_t arm_ttbr0_get()
{
	uint32_t	ttbr0;

	__asm__ __volatile__("mrc p15, 0, %0, c2, c0, 0" :  "=r" (ttbr0));
	return ttbr0;
}
#define arch_pte_read() arm_ttbr0_get()

/*
 * Set TTBR0 register
 */
static inline void arm_ttbr_set(paddr_t ttbr)
{
	const uint32_t zero = 0;

	if (arm_ttbr0_get() == ttbr)
		return;

	__asm__ __volatile__(".align 5");
	__asm__ __volatile__("mcr	p15, 0, %0, c2, c0, 0" : : "r" (ttbr));
	__asm__ __volatile__("mcr	p15, 0, %0, c2, c0, 1" : : "r" (ttbr));
	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");
	__asm__ __volatile__("mcr p15, 0, %0, c7, c5, 0" : : "r" (zero));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0" : : "r" (zero));
	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");
}
#define arch_pte_write(pte) arm_ttbr_set((paddr_t)(pte))

/*
 * Invalidate caches & tlb
 */
static inline void arm_flush_idtlb(void)
{
  const uint32_t zero = 0;

	__asm__ __volatile__(".align 5");
	__asm__ __volatile__("mcr p15, 0, %0, c7, c5, 0" : : "r" (zero));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c6, 0" : : "r" (zero));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0" : : "r" (zero));
	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");
}

static inline void arm_flush_idtlb_addr(vaddr_t addr)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c8, c6, 1" : : "r" (addr));
	__asm__ __volatile__("mcr	p15, 0, %0, c8, c7, 1" : : "r" (addr));
}

/*
 * Set current ASID
 */
static inline void arm_asid_set(uint32_t asid)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c13, c0, 1" : : "r" (asid));
}

/*
 * Flush all TLB entries matching ASID
 */
static inline void arm_tlb_asid(uint32_t asid)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c8, c7, 2" : : "r" (asid));
}

static inline void arm_TPIDRURW_set(uint32_t TPIDRURW)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c13, c0, 2" : : "r" (TPIDRURW));
}
static inline uint32_t arm_TPIDRURW_get()
{
	uint32_t	TPIDRURW;

	__asm__ __volatile__("mrc p15, 0, %0, c13, c0, 2" :  "=r" (TPIDRURW));
	return TPIDRURW;
}

static inline uint32_t arm_ifsr_get()
{
	uint32_t	fault_status;

	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 1" :  "=r" (fault_status));
	return fault_status;
}
static inline uint32_t arm_ifar_get()
{
	uint32_t	fault_addr;

	__asm__ __volatile__("mrc p15, 0, %0, c6, c0, 2" :  "=r" (fault_addr));
	return fault_addr;
}

static inline uint32_t arm_dfsr_get()
{
	uint32_t	fault_status;

	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 0" :  "=r" (fault_status));
	return fault_status;
}
static inline uint32_t arm_dfar_get()
{
	uint32_t	fault_addr;

	__asm__ __volatile__("mrc p15, 0, %0, c6, c0, 0" :  "=r" (fault_addr));
	return fault_addr;
}


static inline void asm_out8(uint16_t port, uint8_t value) {}

static inline uint8_t asm_in8(uint16_t port) { return 0; }

//static inline void asm_cli(void) {}
static inline void disable_interrupt(void )
{
	__asm__ __volatile__("mrs	r0, cpsr");
	__asm__ __volatile__("orr	r0, r0, #0x1c0");	// set A,I,F of cpsr
	__asm__ __volatile__("msr	cpsr, r0");
}

//static inline void asm_sti(void) {}
static inline void enable_interrupt(void )
{
	__asm__ __volatile__("mrs	r0, cpsr");
	__asm__ __volatile__("and	r0, r0, #~0x180");	// clear A,I of cpsr
	__asm__ __volatile__("msr	cpsr, r0");
}

static inline vaddr_t arm_stack(void)
{
	vaddr_t	sp;

	__asm__ __volatile__("mov	%0, sp" : "=r" (sp));
	return sp;
}

static inline uint32_t arm_cpsr_get(void)
{
	uint32_t sr;

	__asm__ __volatile__("mrs	%0, cpsr" : "=r" (sr));
	return sr;
}
static inline void arm_cpsr_set(uint32_t sr)
{
	__asm__ __volatile__("msr	cpsr, %0" : : "r" (sr));
}

static inline uint32_t asm_read_cr2(void) { return 0; }
#define arch_fault_addr() asm_read_cr2()

//static inline void ack_irq(int iIrqVec) {
//    ;	//	write_apic(APIC_REG_EOI, 0);
//}

//inline
void ack_irq(int iIrqVec);


// clang-format on
#define FUNC_CALL_INnOUT(ret_type, name, ...) \
  __attribute__((section(".callino"))) ret_type name(__VA_ARGS__)


static inline void arch_pause() {
	__asm__ __volatile__("");
}

static inline void saveRegsFrame(iframe_t* pPrevFrame, iframe_t* iframe){
	pPrevFrame->spsr= iframe->spsr;		//	uint64_t
	pPrevFrame->r0	= iframe->r0;		//	uint64_t
	pPrevFrame->r1	= iframe->r1;		//	uint64_t
	pPrevFrame->r2	= iframe->r2;		//	uint64_t
	pPrevFrame->r3	= iframe->r3;		//	uint64_t
	pPrevFrame->r4	= iframe->r4;		//	uint64_t
	pPrevFrame->r5	= iframe->r5;		//	uint64_t
	pPrevFrame->r6	= iframe->r6;		//	uint64_t
	pPrevFrame->r7	= iframe->r7;		//	uint64_t
	pPrevFrame->r8	= iframe->r8;		//	uint64_t
	pPrevFrame->r9	= iframe->r9;		//	uint64_t
	pPrevFrame->r10	= iframe->r10;		//	uint64_t
	pPrevFrame->r11	= iframe->r11;		//	uint64_t
	pPrevFrame->r12	= iframe->r12;		//	uint64_t
	pPrevFrame->sp	= iframe->sp;		//	uint64_t
	pPrevFrame->lr	= iframe->lr;		//	uint64_t
	pPrevFrame->pc	= iframe->pc;		//	uint64_t
	//
}

//	The first paramter of the call stack saved in rdi, the second rsi and so on
static inline long getParamStackFirstValue(iframe_t* iframe){return iframe->r0;};
static inline long getParamStackSecondValue(iframe_t* iframe){return iframe->r1;};
static inline long getParamStackThirldValue(iframe_t* iframe){return iframe->r2;};
static inline long getParamStackFourthValue(iframe_t* iframe){return iframe->r3;};
static inline long getParamStackFifthValue(iframe_t* iframe){return iframe->r4;};
static inline long getParamStackSixthValue(iframe_t* iframe){return iframe->r5;};

static inline void setParamStackReturnValue(iframe_t* iframe, long iReturnValue){ iframe->r3=iReturnValue; return;};

#endif
