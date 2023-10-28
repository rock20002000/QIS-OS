/*
 * trap.S
 *
 *  Created on: 2022��11��20��
 *      Author: Test
 */

.text

.global enable_irq
enable_irq:
	MRS    R0, CPSR            // read the CPSR to R0
    BIC    R0, R0,#0xC0     	// clear CPSR I/F ctrl bit
    MSR    CPSR_c,R0          	// write back CPSR
    MOV    PC,LR

.global disable_irq
disable_irq:
    MRS    R0, CPSR             // read the CPSR to R0
    ORR    R0, R0,#0xC0    		// set CPSR I ctrl bit
    MSR    CPSR_c,R0           	// write back CPSR
    MOV    PC,LR


// void arm64_task_switch(vaddr_t *prev_sp, vaddr_t next_sp);
.global arm_task_switch
arm_task_switch:
    // Save callee-saved registers.
	//	stmdb	sp!, {r0-r15}^
	//  stmdb	sp!, {pc, lr}^

    // Switch the stack pointer.
    mov  r9, sp
    str  r9, [r0]
    mov  sp, r1

    // Restore callee-saved registers.
	//	ldmia	sp!, {r0-r15}^
	ldmia	sp!, {pc, lr}
	//	ldmia	sp!, {pc}^

	//	ldmia	sp!, {lr, pc}^

    //mov pc, lr

.global userland_entry					//	arm_start_task
userland_entry:							//	arm_start_task:
	MRS     R0,  CPSR               // Get CPSR of interrupt thread
	bic		R0, R0, #0x1f
	ORR		R0, R0, #0x10
	MSR     SPSR, R0			//MODE_SVC|NOINT ; Switch to SVC mode and no interrupt

	ADD		LR, LR, #4
	SUBS	PC, LR, #4

    //	bl stack_set_canary

    //	ldr  x0, =0 // AArch64, EL0t
    //	msr  spsr_el1, x0

    //	ldr  x0, [sp]
    //	msr  elr_el1, x0
    //	eret


