#define SIGCODE_SIGNO(sigcode)		((sigcode) & 0xff)
#define SIGCODE_CODE(sigcode)		(((sigcode) >> 8) & 0xff)
#define SIGCODE_FAULT(sigcode)		(((sigcode) >> 16) & 0xff)
#define MAKE_SIGCODE(signo,code,fault) ((signo) | ((code)<<8) | ((fault)<<16))
#define SIGCODE_FLAGS_MASK			0xff000000
#define SIGCODE_FATAL				0x01000000
#define SIGCODE_INXFER				0x01000000 /* for use in fault handling */
#define SIGCODE_INTR				0x02000000
#define SIGCODE_KEREXIT				0x02000000 /* for use in fault handling */
#define SIGCODE_KERNEL				0x04000000
#define SIGCODE_USER				0x08000000
#define SIGCODE_PROC				0x10000000
#define SIGCODE_BDSLOT				0x20000000
#define SIGCODE_STORE				0x80000000
#define SIGCODE_SSTEP				0x80000000 /* for use with F.P. emulator */

#define SIG_ERR             ((void(*)(_SIG_ARGS)) -1)
#define SIG_DFL             ((void(*)(_SIG_ARGS))  0)
#define SIG_IGN             ((void(*)(_SIG_ARGS))  1)
#define SIG_HOLD            ((void(*)(_SIG_ARGS))  2)

#define SIGHUP      1   /* hangup */
#define SIGINT      2   /* interrupt */
#define SIGQUIT     3   /* quit */
#define SIGILL      4   /* illegal instruction (not reset when caught) */
#define SIGTRAP     5   /* trace trap (not reset when caught) */
#define SIGIOT      6   /* IOT instruction */
#define SIGABRT     6   /* used by abort */
#define SIGEMT      7   /* EMT instruction */
#define SIGDEADLK	7	/* Mutex deadlock */
#define SIGFPE      8   /* floating point exception */
#define SIGKILL     9   /* kill (cannot be caught or ignored) */
#define SIGBUS      10  /* bus error */
#define SIGSEGV     11  /* segmentation violation */
#define SIGSYS      12  /* bad argument to system call */
#define SIGPIPE     13  /* write on pipe with no reader */
#define SIGALRM     14  /* real-time alarm clock */
#define SIGTERM     15  /* software termination signal from kill */
#define SIGUSR1     16  /* user defined signal 1 */
#define SIGUSR2     17  /* user defined signal 2 */
#define SIGCHLD     18  /* death of child */
#define SIGCLD      SIGCHLD
#define SIGPWR      19  /* power-fail restart */
#define SIGWINCH    20  /* window change */
#define SIGURG      21  /* urgent condition on I/O channel */
#define SIGPOLL     22  /* System V name for SIGIO */
#define SIGIO       SIGPOLL
#define SIGSTOP     23  /* sendable stop signal not from tty */
#define SIGTSTP     24  /* stop signal from tty */
#define SIGCONT     25  /* continue a stopped process */
#define SIGTTIN     26  /* attempted background tty read */
#define SIGTTOU     27  /* attempted background tty write */
#define SIGVTALRM   28  /* virtual timer expired */
#define SIGPROF     29  /* profileing timer expired */
#define SIGXCPU     30  /* exceded cpu limit */
#define SIGXFSZ     31  /* exceded file size limit */

#define	ARM_REG_R0		0
#define	ARM_REG_R1		1
#define	ARM_REG_R2		2
#define	ARM_REG_R3		3
#define	ARM_REG_R4		4
#define	ARM_REG_R5		5
#define	ARM_REG_R6		6
#define	ARM_REG_R7		7
#define	ARM_REG_R8		8
#define	ARM_REG_R9		9
#define	ARM_REG_R10		10
#define	ARM_REG_R11		11
#define	ARM_REG_R12		12
#define	ARM_REG_R13		13
#define	ARM_REG_R14		14
#define	ARM_REG_R15		15
#define	ARM_REG_SPSR	16

/*
 * Register name aliases
 */
#define	ARM_REG_FP		11
#define	ARM_REG_IP		12
#define	ARM_REG_SP		13
#define	ARM_REG_LR		14
#define	ARM_REG_PC		15

#define ARM_CPSR_MODE_MASK		0x1f
#define ARM_CPSR_MODE_USR		0x10
#define ARM_CPSR_MODE_FIQ		0x11
#define ARM_CPSR_MODE_IRQ		0x12
#define ARM_CPSR_MODE_SVC		0x13
#define ARM_CPSR_MODE_ABT		0x17
#define ARM_CPSR_MODE_UND		0x1b
#define ARM_CPSR_MODE_SYS		0x1f
#define ARM_CPSR_T				(1 << 5)
#define ARM_CPSR_F				(1 << 6)
#define ARM_CPSR_I				(1 << 7)
#define	ARM_CPSR_A				(1 << 8)
#define	ARM_CPSR_E				(1 << 9)
#define	ARM_CPSR_GE_MASK		0x000f0000
#define	ARM_CPSR_GE0			(1 << 16)
#define	ARM_CPSR_GE1			(1 << 17)
#define	ARM_CPSR_GE2			(1 << 18)
#define	ARM_CPSR_GE3			(1 << 19)
#define	ARM_CPSR_J				(1 << 24)
#define	ARM_CPSR_Q				(1 << 27)
#define ARM_CPSR_COND_MASK		0xf0000000
#define ARM_CPSR_V				(1 << 28)
#define ARM_CPSR_C				(1 << 29)
#define ARM_CPSR_Z				(1 << 30)
#define ARM_CPSR_N				(1 << 31)

#define ARM_CPSR_FIQ			(ARM_CPSR_F | ARM_CPSR_I | ARM_CPSR_MODE_FIQ)
#define ARM_CPSR_IRQ			(ARM_CPSR_F | ARM_CPSR_I | ARM_CPSR_MODE_IRQ)
#define ARM_CPSR_UND			(ARM_CPSR_F | ARM_CPSR_I | ARM_CPSR_MODE_UND)
#define ARM_CPSR_ABT			(ARM_CPSR_F | ARM_CPSR_I | ARM_CPSR_MODE_ABT)
#define ARM_CPSR_SYS			(ARM_CPSR_F | ARM_CPSR_I | ARM_CPSR_MODE_SYS)

#ifdef	__PIC__
#define	FUNC_ENTER	stmdb	sp!, {sl,lr}
#define	FUNC_RETURN	ldmia	sp!, {sl,pc}
#else
#define	FUNC_ENTER	stmdb	sp!, {lr}
#define	FUNC_RETURN	ldmia	sp!, {pc}
#endif

#define	ARM_GICD_ISENABLERn		0x100


.extern	bootstrap_core_init

.extern handle_reset_exception
.extern handle_undefined_exception
.extern arm_handle_syscall
//.extern handle_prefetch_exception
//.extern handle_abort_exception

.extern handle_prefetch_abort
.extern handle_data_abort

.extern handle_reserve_exception
.extern arm_handle_interrupt

.extern stackUND
.extern stackIRQ
.extern stackFIQ
.extern stackABT

.extern kernel_stack_syscall_global
.extern kernel_stack_interrupt_global
.extern g_gicvaddr

.globl	_entry

.section ".text", "ax"

# The entry point jumped from the bsp,we assume that bsp has already constructed the basic page settings.
.global _entry
_entry:
	mrs		r6, cpsr	// backup svc mode

	msr		cpsr, ARM_CPSR_FIQ
	ldr		sp, =stackFIQ

	msr		cpsr, ARM_CPSR_IRQ
	ldr		sp, =stackIRQ

	msr		cpsr, ARM_CPSR_UND
	ldr		sp, =stackUND

	msr		cpsr, ARM_CPSR_ABT
	ldr		sp, =stackABT

	msr		cpsr, ARM_CPSR_SYS
	setend	le

	msr		cpsr, r6	// restore svc mode

    bl		bootstrap_core_init

halt:
	b		halt


.global user_entry
user_entry:
	msr		cpsr, ARM_CPSR_MODE_SVC
	ldr		sp, =kernel_stack_syscall_global
 	ldr		sp, [sp]

	msr		cpsr, ARM_CPSR_MODE_IRQ
	ldr		sp, =kernel_stack_interrupt_global
 	ldr		sp, [sp]
	
	MRS     R0,  CPSR               // Get CPSR of interrupt thread
	bic		R0, R0, #0x1f
	ORR		R0, R0, #0x10
	MSR     SPSR, R0			//MODE_SVC|NOINT ; Switch to SVC mode and no interrupt

	ADD		LR, LR, #4
	SUBS	PC, LR,#4


.align 2
.globl	init_traps
init_traps:
	/*
	 * Copy trap handler entry points.
	 * The startup program has set each entry point to "ldr pc, [pc, #0x18]"
	 *
	 * If MMU X bit (bit 13) is set, vectors are at 0xffff0000
	 */
	//ldr	r1, =0xffff0020
	mov		r1, #32						//	#64
	mrc		p15, 0, r0, c1, c0, 0
	tst		r0, #(1 << 13)
	orrne	r1, r1, #0xff000000
	orrne	r1, r1, #0x00ff0000
	adr		r0, __trap_vectors
	mov		r2, #8
0:	ldr		ip, [r0], #4
	str		ip, [r1], #4
	subs	r2, r2, #1
	bne		0b

	ldr r6,=0xffff0000			//	__trap_vectors		//	__ram_vectors_start
	MCR p15,0,r6,c12,c0,0
	MCR p15,0,r6,c12,c0,1

	mov		pc, lr

__trap_vectors:
	.word	__rst_entry
	.word	__und_entry
	.word	__swi_entry
	.word	__prf_entry
	.word	__abt_entry
	.word	__rsv_entry
	.word	__irq_entry
	.word	__fiq_entry


__sigtrap: .word SIGTRAP | SIGCODE_KERNEL
__sigsegv: .word SIGSEGV | SIGCODE_KERNEL
__sigint:  .word SIGINT  | SIGCODE_KERNEL
__sigsys:  .word SIGSYS  | SIGCODE_KERNEL

.macro	SAVE_CONTEXT pc_adjust
	/*
	 * Save context on gdb stack
	 */
	//ldr		sp, =_stack_top
	//ldr		sp, [sp]
	sub		sp, sp, #17*4

	/*
	 * Non-banked registers
	 */
	stmia	sp, {r0-r12}

	/*
	 * Return pc and status register
	 */
	sub		lr, lr, #&pc_adjust
	str		lr, [sp, #ARM_REG_PC*4]
	mrs		r0, spsr
	str		r0, [sp, #ARM_REG_SPSR*4]

	/*
	 * Switch to SVC mode and save banked registers
	 */
	mov		r2, sp
	add		r3, sp, #ARM_REG_SP*4
	mrs		r1, cpsr
	bic		r1, r1, #ARM_CPSR_MODE_MASK
	orr		r1, r1, #ARM_CPSR_MODE_SVC
	and		r0, r0, #ARM_CPSR_MODE_MASK
	teq		r0, #ARM_CPSR_MODE_SVC
	stmeqia	r3, {sp,lr}
	stmneia	r3, {sp,lr}^
.endm

.macro	RESTORE_CONTEXT
	ldr		r0, [sp, #ARM_REG_SPSR*4]
	msr		spsr, r0
	mov		r0, sp
	ldmia	r0, {r0-r15}^
.endm

.macro	GETCPU reg
	mrc		p15, 0, &reg, c0, c0, 5			// get cpuid
	and		&reg, &reg, #0xf
.endm

.macro	SMPLDR	dst, src, cpu, cc=
	ldr&cc	&dst, [&src, &cpu, lsl #2]		// dst = src[cpu]
.endm


/*
 * Reset exception (branch though zero)
 */
__rst_entry:
 	ldr		sp, =kernel_stack_interrupt_global	//	stackABT		//	kernel_stack_global
 	ldr		sp, [sp]

	SAVE_CONTEXT 4
	//mov 	r0, #0x82
	//bl		put

	mov		r0, #0
	ldr		r1, __sigtrap
	mov		sp, r2
	bl		handle_reset_exception
	RESTORE_CONTEXT

/*
 * Undefined instruction exception
 */
__und_entry:
    sub     lr, lr, #4
    stmfd   sp!, {lr}
    sub     sp, #4*13
    stmia   sp, {r0-r12}
    mrs     r2, spsr
    msr     cpsr, ARM_CPSR_SYS
    mov     r1, lr	/* get lr of user mode */
    mov     r0, sp	/* get sp of user mode */
    msr     cpsr, ARM_CPSR_ABT
    stmfd   sp!, {r0-r2}
    mov     r0, sp	/* iframe_t */
    bl      handle_undefined_exception
    LDMFD   sp!, {r0-r2}
    msr     spsr, r2
    LDMFD   sp!, {r0-r12, pc}^

/*
 * SWI instruction
 */
 
 __swi_entry:
 	ldr		sp, =kernel_stack_syscall_global	//	stackSVC			//	kernel_stack_global
 	ldr		sp, [sp]
    //sub     lr, lr, #4
    //stmfd   sp!, {lr}
   	//STMFD   SP!,{R0-R12}			//
   	STMFD   SP!,{R0-R12, LR}	//STMFD   SP!,{R0-R3,R12,LR}		//	protect environment {r4~r11} protected by arm
    MOV     r1,sp;					// 	if the SWI call with param set R1 as the first parameter
    MRS     R0,SPSR 				//	read SPSR
    //bic		R0, #0xf
    STMFD   SP!,{R0}				//  save SPSR

  	msr		cpsr, 0xdf				//	ARM_CPSR_SYS
	mov		r0, sp
	mov 	r1, lr
	msr		cpsr, 0xd3				//	0x13
	stmfd   sp!, {r0, r1}

    mov		r0, sp
    BL      arm_handle_syscall		//	handle_swi_exception	//	SWI_Exception_Function //	;

	ldmfd   sp!, {r0, r1}
	msr		cpsr, 0xdf				//	ARM_CPSR_SYS
	mov		sp, r0
	mov 	lr, r1
	msr		cpsr, 0xd3				//	0x13

    LDMFD   sp!,  {r0}
    MSR     spsr_cf,r0              // ;// restore spsr
    LDMFD   sp!,  {r0-r12, pc}^ //  ;// restore env

/*
 * Prefetch abort exception
 */
__prf_entry:
    sub     lr, lr, #4
    stmfd   sp!, {lr}
    sub     sp, #4*13
    stmia   sp, {r0-r12}
    mrs     r2, spsr
    msr     cpsr, ARM_CPSR_SYS
    mov     r1, lr	/* get lr of user mode */
    mov     r0, sp	/* get sp of user mode */
    msr     cpsr, ARM_CPSR_ABT
    stmfd   sp!, {r0-r2}
    mov     r0, sp	/* iframe_t */
    bl      handle_prefetch_abort
    LDMFD   sp!, {r0-r2}
    msr     spsr, r2
    LDMFD   sp!, {r0-r12, pc}^

/*
 * Data abort exception
 */
__abt_entry:
    sub     lr, lr, #8
    stmfd   sp!, {lr}
    sub     sp, #4*13
    stmia   sp, {r0-r12}
    mrs     r2, spsr
    msr     cpsr, ARM_CPSR_SYS
    mov     r1, lr	/* get lr of user mode */
    mov     r0, sp	/* get sp of user mode */
    msr     cpsr, ARM_CPSR_ABT
    stmfd   sp!, {r0-r2}
    mov     r0, sp	/* iframe_t */
    bl      handle_data_abort
    LDMFD   sp!, {r0-r2}
    msr     spsr, r2
    LDMFD   sp!, {r0-r12, pc}^

/*
 * Reserved exception
 */
__rsv_entry:
 	ldr		sp, =kernel_stack_interrupt_global	//	stackABT		//	kernel_stack_global
 	ldr		sp, [sp]
	//mov 	r0, #115	//	a
	//bl		put

	SAVE_CONTEXT 8
	mov		r0, #0
	ldr		r1, __sigsegv
	mov		sp, r2
	bl		handle_reserve_exception
	RESTORE_CONTEXT

/*
 * IRQ interrupt
 */
 
__irq_entry:
 	ldr		sp, =kernel_stack_interrupt_global	//	stackSVC			//	kernel_stack_global
 	ldr		sp, [sp]
    sub     lr, lr, #4
    stmfd   sp!, {lr}
   	STMFD   SP!,{R0-R12}	//STMFD   SP!,{R0-R12, LR}	//STMFD   SP!,{R0-R3,R12,LR}		//	protect environment {r4~r11} protected by arm
    MOV     r1,sp;					// 	if the SWI call with param set R1 as the first parameter
    MRS     R0,SPSR 				//	read SPSR
    //bic		R0, #0xf
    STMFD   SP!,{R0}				//  save SPSR

  	msr		cpsr, 0xdf				//	ARM_CPSR_SYS
	mov		r0, sp
	mov 	r1, lr

	msr		cpsr, 0xd2				//	0x12
	stmfd   sp!, {r0, r1}
 	mov		r1, sp

    //ldr r2, =g_gicvaddr
    //ldr r2, [r2]
	//add r2, r2, #0x0100			// GIC base plus 0x0100 is the GIC CPU base address
	//ldr r0, [r2, #0xC]			// GIC CPU shift 0x0C is the GICC_IAR reg
								// GICC_IAR keeps the interrupt No.
								// The interrupt No. is used to call interrupt service routine
								//
	push {r0, r2}				// save r0,r2

	cps #0x13					// enter SVC mode, enable other irqs

	push {lr}					// save SVC mode lr reg
	ldr r3, =arm_handle_interrupt	// load interrupt handler to r2
	blx r3						// run interrupt handler with parameter in r0

	pop {lr}					//  pop lr
	cps #0x12					// enter IRQ mode
	pop {r0, r2}
	//str r0, [r2, #0x10]			// finish interrupt handler, write back EOIR

	ldmfd   sp!, {r0, r1}
	msr		cpsr, 0xdf				//	ARM_CPSR_SYS
	mov		sp, r0
	mov 	lr, r1
	msr		cpsr, 0xd2			//	0x12

    LDMFD   sp!,  {r0}
    MSR     spsr_cf,r0              // ;// restore spsr
    LDMFD   sp!,  {r0-r12, pc}^ //  ;// restore env

__fiq_entry:
 	ldr		sp, =kernel_stack_interrupt_global	//	stackFIQ		//	kernel_stack_global
 	ldr		sp, [sp]

	//mov 	r0, #102	// f
	//bl		put

	SAVE_CONTEXT 4
	mov		r0, #0
	ldr		r1, __sigint
	mov		sp, r2
	bl		arm_handle_interrupt
	RESTORE_CONTEXT
