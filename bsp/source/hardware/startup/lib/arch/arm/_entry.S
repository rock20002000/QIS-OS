/*
 * arm_entry.S
 *
 *	Initial code to get Neutrino started
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 */

.extern	_main
.extern	__boot_stack_base
.globl	_entry
	
.section ".boot", "ax"
_entry:


	ldr		sp, =__boot_stack_base + 0x1000*1
	bl		_main

halt:
	b		halt
