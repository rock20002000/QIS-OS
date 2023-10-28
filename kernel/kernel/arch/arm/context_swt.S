
.section .text, "ax"
/*
 * phy_base_t phy_hw_interrupt_disable();
 */
.globl phy_hw_interrupt_disable
phy_hw_interrupt_disable:
    mrs r0, cpsr
    cpsid i
    bx  lr

/*
 * void phy_hw_interrupt_enable(phy_base_t level);
 */
.globl phy_hw_interrupt_enable
phy_hw_interrupt_enable:
    msr cpsr, r0
    bx  lr

/*
 * void rt_hw_context_switch_to(rt_uint32 to);
 * r0 --> to
 */
.globl phy_hw_context_switch_to
phy_hw_context_switch_to:
    ldr sp, [r0]            @ get new task stack pointer

    ldmfd sp!, {r4}         @ pop new task spsr
    msr spsr_cxsf, r4

    ldmfd sp!, {r0-r12, lr, pc}^   @ pop new task r0-r12, l    

/*
 * void phy_hw_context_switch(phy_uint32 from, phy_uint32 to);
 * r0 --> from
 * r1 --> to
 */
.globl phy_hw_context_switch
phy_hw_context_switch:
    stmfd   sp!, {lr}       @ push pc (lr should be pushed in place of PC)
    stmfd   sp!, {r0-r12, lr}   @ push lr & register file

    mrs r4, cpsr
    tst lr, #0x01
    orrne r4, r4, #0x20     @ it's thumb code

    stmfd sp!, {r4}         @ push cpsr

    str sp, [r0]            @ store sp in preempted tasks TCB
    ldr sp, [r1]            @ get new task stack pointer

    ldmfd sp!, {r4}         @ pop new task cpsr to spsr
    msr spsr_cxsf, r4
    ldmfd sp!, {r0-r12, lr, pc}^  @ pop new task r0-r12, lr & pc, copy spsr to cpsr

/*
 * void phy_hw_context_switch_interrupt(phy_uint32 from, phy_uint32 to);
 */
.globl phy_thread_switch_interrupt_flag
.globl phy_interrupt_from_thread
.globl phy_interrupt_to_thread
.globl phy_hw_context_switch_interrupt
phy_hw_context_switch_interrupt:
    ldr r2, =phy_thread_switch_interrupt_flag
    ldr r3, [r2]
    cmp r3, #1
    beq _reswitch
    ldr ip, =phy_interrupt_from_thread   @ set phy_interrupt_from_thread
    mov r3, #1              @ set phy_thread_switch_interrupt_flag to 1
    str r0, [ip]
    str r3, [r2]
_reswitch:
    ldr r2, =phy_interrupt_to_thread     @ set phy_interrupt_to_thread
    str r1, [r2]
    bx  lr
