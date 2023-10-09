/*
 * arm_entry.S
 *
 *	Initial code to get Neutrino started
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 */


	.extern	_boot_main
	.globl	_entry

.section ".boot", "ax"
_entry:
	b		_text_start

.section ".magic"	
	.ascii "origincode"

.section ".text", "ax"
_text_start:
	/*
	 * Turn off interrupts and make sure we are in SVC mode
	 */
#if 0
	mrs		lr, cpsr
	bic		lr, lr, #0x1f
	orr		lr, lr, #0xd3
	msr		cpsr, lr
#else
	mov		lr, #0x1d3
	msr		cpsr, lr
#endif

	/* Halt cpu if it's not boot core */
	mrc		p15, 0, r0, c0, c0, 5
	and		r0, r0, #0xFF
	cmp		r0, #0
	bne		halt
#if 0
	/*
	 * Turn off MMU and data cache if necessary.
	 * WARNING: assumes we are running with a 1-1 mapping if MMU is enabled.
	 */
	mrc		p15, 0, lr, c1, c0, 0
	orr		lr, #4						// Cache enable 
	//bic		lr, lr, #0x00000001			// WCAM bits
	//bic		lr, lr, #0x00c00000			// WCAM bits
	bic		lr, lr, #0x0000000f			// WCAM bits
	bic		lr, lr, #0x00000300			// WCAM bits
	mov		r0, #0
	/*
	 * Invalidate instruction cache and TLBs.
	 * WARNING: assumes data caches are clean.
	 */
	//.align  5
	dsb
	isb
	mcr		p15, 0, lr, c1, c0, 0
	mcr		p15, 0, r0, c7, c5, 0		// invalidate instruction caches
	mcr		p15, 0, r0, c8, c7, 0		// invalidate TLBs
	dsb
	isb
	#endif

	
	/*
	 * Caches are in an undefined state on cold reset so invalidate them
	 */
	mov		r0, #0
	mcr		p15, 2, r0, c0, c0, 0
	mcr		p15, 0, ip, c7, c5, 0
	mrc		p15, 1, r0, c0, c0, 0

	ldr		r1, =0x7fff
	and		r2, r1, r0, lsr #13

	ldr		r1, =0x3ff
	and		r3, r1, r0, lsr #3	// NumWays - 1
	add		r2, r2, #1			// NumSets

	and		r0, r0, #0x7
	add		r0, r0, #4			// SetShift

	clz		r1, r3				// WayShift
	add		r4, r3, #1			// NumWays
1:	sub		r2, r2, #1			// NumSets--
	mov		r3, r4				// Temp = NumWays
2:	subs	r3, r3, #1			// Temp--
	mov		r5, r3, lsl r1
	mov		r6, r2, lsl r0
	orr		r5, r5, r6			// Reg = (Temp<<WayShift)|(NumSets<<SetShift)
	mcr		p15, 0, r5, c7, c6, 2
	bgt		2b
	cmp		r2, #0
	bgt		1b
	dsb
	isb


	ldr		sp, =__boot_stack_top
	
	/* Initialize bss section */
	bl 		clear_bss


	bl		_boot_main

halt:
	b		halt


clear_bss:
 	ldr r0,=__bss
	ldr r1,=__bss_end
	cmp r0,r1
	moveq pc,lr

clean_loop:
	mov r2,#0
	str r2,[r0],#4
	cmp r0,r1
	bne clean_loop
	mov pc,lr
