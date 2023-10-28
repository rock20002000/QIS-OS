.section ".text", "ax"

# Will jump to entry address and never return;

.global start_next

start_next:
    # The boot info address is stored in r0,CPU index in r1 and kernel entry is stored in r2

	teq		r3, #0
	beq		0f
	mov		sp, r3
0:
    # Just set pc = r2 to enter bsp code
    /*
        * Call entry_point(prt_bootinfo)
    */
    mov		pc, r2


.align 2
.globl mmu_enable
/*
 * void	mmu_enable(uint32_t ttbr, uint32_t mmu_cr_clr, uint32_t mmu_cr_set)
 *
 * Enable the mmu
 * The next program is responsible for tearing down the 1-1 section
 */
mmu_enable:
	/*
	 * Set the translation table base
	 */
	mov		ip, r0
	mcr		p15, 0, ip, c2, c0, 0		// TTBR0
	mcr		p15, 0, ip, c2, c0, 1		// TTBR1
	mov		ip, #0
	mcr		p15, 0, ip, c2, c0, 2		// TTBCR = 0, TTBR1 not used

	/*
	 * Enable MMU domain 0
	 */
	mov		ip, #1
	mcr		p15, 0, ip, c3, c0, 0

	/*
	 * Enable the MMU, using read-modify-write to preserve reserved bits.
	 */
	mov		r0, lr
	mrc		p15, 0, r0, c1, c0, 0
	bic		ip, r0, r1
	orr		ip, ip, r2
	dsb
	isb
	mcr		p15, 0, ip, c1, c0, 0
	dsb
	isb

	mov		ip, #0

	/*
	 * Invalidate the caches and TLBs
	 */
	.align 5
	mcr		p15, 0, ip, c7, c5, 0	// invalidate instruction caches
	mcr		p15, 0, ip, c8, c7, 0	// invalidate TLBs
	dsb
	isb

	bx		lr
