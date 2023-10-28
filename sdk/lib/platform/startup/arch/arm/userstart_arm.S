/*
 * userstart_arm.S
 *
 *  Created on: 2022��11��14��
 *      Author: Test
 */

.global main
.global _start

.section ".init"
_start:
	setend	le
	ldr	sp, =__stack_end
	bl proc_entry

run_end:
	b run_end

