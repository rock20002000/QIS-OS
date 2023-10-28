#ifndef __MMU_ARM32_H__
#define __MMU_ARM32_H__

/*
 * MMU control register
 */
#define ARM_MMU_CR_M    (1 << 0)  /* enable MMU */
#define ARM_MMU_CR_A    (1 << 1)  /* enable alignment faults */
#define ARM_MMU_CR_C    (1 << 2)  /* enable data/IDC cache */
#define ARM_MMU_CR_W    (1 << 3)  /* enable write buffer */
#define ARM_MMU_CR_P    (1 << 4)  /* 32-bit exception handling */
#define ARM_MMU_CR_D    (1 << 5)  /* 32-bit data address range */
#define ARM_MMU_CR_L    (1 << 6)  /* late aborts */
#define ARM_MMU_CR_B    (1 << 7)  /* big endian */
#define ARM_MMU_CR_S    (1 << 8)  /* system protection */
#define ARM_MMU_CR_R    (1 << 9)  /* ROM protection */
#define ARM_MMU_CR_F    (1 << 10) /* implementation defined */
#define ARM_MMU_CR_Z    (1 << 11) /* implementation defined */
#define ARM_MMU_CR_I    (1 << 12) /* enable instruction cache */
#define ARM_MMU_CR_X    (1 << 13) /* exception vector adjust */
#define ARM_MMU_CR_V    ARM_MMU_CR_X
#define ARM_MMU_CR_RR   (1 << 14) /* round robin cache replacement */
#define ARM_MMU_CR_L4   (1 << 15) /* load PC instr can set T bit */
#define ARM_MMU_CR_DT   (1 << 16) /* data TCM enable */
/* reserved				(1 << 17) */
#define ARM_MMU_CR_IT   (1 << 18) /* instruction TCM enable */
/* reserved				(1 << 19) */
/* reserved				(1 << 20) */
#define ARM_MMU_CR_FI   (1 << 21) /* fast interrupt config enable */
#define ARM_MMU_CR_U    (1 << 22) /* unaligned/mixed-endian enable */
#define ARM_MMU_CR_XP   (1 << 23) /* enable ARM page table descriptors */
#define ARM_MMU_CR_VE   (1 << 24) /* vectored interrupt enable */
#define ARM_MMU_CR_EE   (1 << 25) /* mixed-endian exception entry */
#define ARM_MMU_CR_L2   (1 << 26) /* L2 unified cache enable */
#define ARM_MMU_CR_NMFI (1 << 27) /* FIQs behave as NMFI */
#define ARM_MMU_CR_TR   (1 << 28) /* use TEX remap registers */
#define ARM_MMU_CR_FA   (1 << 29) /* force use of AP[0] as access bit */
#define ARM_MMU_CR_nF   (1 << 30) /* ARM920 only: not FastBus select */
#define ARM_MMU_CR_iA   (1 << 31) /* ARM920 only: async clock select */

#define ARM_L1_PAGES    4
#define ARM_L1_SIZE     (PAGE_SIZE * ARM_L1_PAGES)
#define	ARM_L2_SHIFT	(10)
#define ARM_L2_SIZE     (1 << ARM_L2_SHIFT)
#define	ARM_L2_MASK		(ARM_L2_SIZE - 1)

#define	ARM_SC_SHIFT	20
#define	ARM_SCSIZE		(1 << 20)
#define	ARM_SCMASK		(ARM_SCSIZE - 1)
#define ARM_SC_CNT(siz) (((siz) + ARM_SCMASK) >> ARM_SC_SHIFT)

#define ARM_PTE_B       4   /* bufferable */
#define ARM_PTE_C       8   /* machine-dependent (see below) */
#define ARM_PTE_CB      12 /* cacheable and bufferable */

/*
 * L1 descriptors
 */
#define ARM_PTP_NULL	0
#define ARM_PTP_L2      1	/* ARM L1 descriptor */
#define ARM_PTP_SC      2	/* ARM L1 descriptor */
#define ARM_PTP_MASK	3
#define ARM_PTP_VALID   3

#define ARM_PTP_P       (1 << 9)      /* ECC enabled */
#define ARM_PTP_SUPER   (1 << 18) /* supersection descriptor */

/*
 * L1 section cacheability
 */
#define ARM_PTP_B       4           /* bufferable */
#define ARM_PTP_C       8           /* machine-dependent (see below) */
#define ARM_PTP_CB      12          /* cacheable and bufferable */
#define ARM_PTP_WB      ARM_PTP_CB  /* write-back cacheable */
#define ARM_PTP_WT      ARM_PTP_C   /* write-thru cacheable */

/*
 * ARM L1 descriptor bits
 */
#define ARM_PTP_XN   (1 << 4)
#define ARM_PTP_AP0  (1 << 10)
#define ARM_PTP_AP1  (1 << 11)
#define ARM_PTP_APX  (1 << 15)
#define ARM_PTP_S    (1 << 16)
#define ARM_PTP_nG   (1 << 17)

#define ARM_PTP_DOMAIN(x)	(((x)&15) << 4)
#define ARM_PTP_TEX(x)      (((x)&7) << 12)
#define ARM_PTP_TEX_MASK    ARM_PTP_TEX(7)

/*
 * ARM section access permissions
 *
 * APX bit determines whether write access is allowed
 * AP1 bit determines whether user access is allowed
 *
 * The encodings below reflect the ARM encoding in ARMARMrevF:
 *
 * APX AP1 AP0
 * --- --- ---
 *   1   0   1	priv RO / user NA
 *   0   0   1	priv RW / user NA
 *   1   1   0	priv RO / user RO
 *   0   1   1	priv RW / user RW
 */
#define ARM_PTP_AP_U    ARM_PTP_AP1
#define ARM_PTP_AP_URO  (ARM_PTP_AP1 | ARM_PTP_APX)
#define ARM_PTP_AP_URW  (ARM_PTP_AP1 | ARM_PTP_AP0)
#define ARM_PTP_AP_KRO  (ARM_PTP_AP0 | ARM_PTP_APX)
#define ARM_PTP_AP_KRW  (ARM_PTP_AP0)

/*
 * L2 descriptors
 */
#define ARM_PTE_NULL	0
#define ARM_PTE_LP      1
#define ARM_PTE_SP      2
#define ARM_PTE_XSP     3 /* ARMv5 extended small page */
#define ARM_PTE_MASK	3
#define ARM_PTE_VALID   3

#define ARM_PTE_PROT(f) (((f) << 4) | ((f) << 6) | ((f) << 8) | ((f) << 10))
#define ARM_XSP_PROT(f) ((f) << 4)

#define ARM_PTE_WT      ARM_PTE_C    /* write-thru (non-SA11x0) */
#define ARM_PTE_WB      ARM_PTE_CB   /* write-back (all cores)  */
#define ARM_PTE_SA_MC   ARM_PTE_C /* mini-cache (SA11x0)     */

#define ARM_PTE_XS_X    0x40                        /* Xscale X-bit */
#define ARM_PTE_XS_MC   (ARM_PTE_XS_X | ARM_PTE_C)  /* mini-cache (Xscale) */
#define ARM_PTE_XS_WA   (ARM_PTE_XS_X | ARM_PTE_WB) /* write-allocate */

#define ARM_PTE_AP0     (1 << 4)
#define ARM_PTE_AP1     (1 << 5)
#define ARM_PTE_APX     (1 << 9)
#define ARM_PTE_S       (1 << 10)
#define ARM_PTE_nG      (1 << 11)

/*
 * APX bit determines whether write access is allowed
 * AP1 bit determines whether user access is allowed
 *
 * The encodings below reflect the ARM encoding in ARMARMrevF:
 *
 * APX AP1 AP0
 * --- --- ---
 *   1   0   1	priv RO / user NA
 *   0   0   1	priv RW / user NA
 *   1   1   0	priv RO / user RO
 *   0   1   1	priv RW / user RW
 */
#define ARM_PTE_AP_U        ARM_PTE_AP1
#define ARM_PTE_AP_URO      (ARM_PTE_AP1 | ARM_PTE_APX)
#define ARM_PTE_AP_URW      (ARM_PTE_AP1 | ARM_PTE_AP0)
#define ARM_PTE_AP_KRO      (ARM_PTE_AP0 | ARM_PTE_APX)
#define ARM_PTE_AP_KRW      (ARM_PTE_AP0)
#define ARM_PTE_AP_MASK     (ARM_PTE_AP0 | ARM_PTE_AP1 | ARM_PTE_APX | ARM_PTE_nG)

#define ARM_PTE_SP_XN       (1 << 0) /* XN for small pages only */
#define ARM_PTE_SP_TEX(x)   (((x)&7) << 6)
#define ARM_PTE_SP_TEX_MASK ARM_PTE_SP_TEX(7)

#define ARM_PTE_LP_XN       (1 << 15)
#define ARM_PTE_LP_TEX(x)   (((x)&7) << 12)
#define ARM_PTE_LP_TEX_MASK ARM_PTE_LP_TEX(7)

#define ARM_PTE_WA          (ARM_PTE_SP_TEX(1) | ARM_PTE_CB)

/*
 * ARM TTBR0/TTBR1 attributes
 */
#define ARM_TTBR_ATTR_MASK	0x7f
#define	ARM_TTBR_C			(1 << 0)			/* inner cacheable */
#define	ARM_TTBR_S			(1 << 1)			/* shared memory */
#define	ARM_TTBR_P			(1 << 2)			/* ECC enabled */
#define	ARM_TTBR_RGN(x)     (((x) & 3) << 3)	/* outer cacheable type */
#define ARM_TTBR_IRGN(x)    ((((x) & 1) << 6) | (((x) & 2) >> 1))
#define ARM_TTBR_NOS        (1 << 5)

#define ARM_TTBR_RGN_NC     ARM_TTBR_RGN(0)
#define ARM_TTBR_RGN_WA     ARM_TTBR_RGN(1)
#define ARM_TTBR_RGN_WT     ARM_TTBR_RGN(2)
#define ARM_TTBR_RGN_WB     ARM_TTBR_RGN(3)

#define ARM_TTBR_IRGN_NC    ARM_TTBR_IRGN(0)
#define ARM_TTBR_IRGN_WA    ARM_TTBR_IRGN(1)
#define ARM_TTBR_IRGN_WT    ARM_TTBR_IRGN(2)
#define ARM_TTBR_IRGN_WB    ARM_TTBR_IRGN(3)

#define ARM_MMU_PTE_S       (ARM_PTE_SP | ARM_PTE_SP_TEX(1) | ARM_PTE_S)
#define ARM_MMU_PTE_C       (ARM_MMU_PTE_S | ARM_PTE_C)
#define ARM_MMU_PTE_CB      (ARM_MMU_PTE_C | ARM_PTE_B)
#define ARM_MMU_PTE_KU      ARM_MMU_PTE_C

#define ARM_MMU_UPTE_RO     (ARM_MMU_PTE_KU | ARM_PTE_AP_URO | ARM_PTE_nG)
#define ARM_MMU_UPTE_RW     (ARM_MMU_PTE_KU | ARM_PTE_AP_URW | ARM_PTE_nG)
#define ARM_MMU_KPTE_RO     (ARM_MMU_PTE_KU | ARM_PTE_AP_KRO)
#define ARM_MMU_KPTE_RW     (ARM_MMU_PTE_KU | ARM_PTE_AP_KRW)
#define ARM_MMU_MASK_NC     (ARM_PTE_CB | ARM_PTE_SP_TEX_MASK)
#define ARM_MMU_L1_PGTABLE  (ARM_PTP_L2)
#define ARM_MMU_USCN_RO     (ARM_PTP_SC | ARM_PTP_AP_URO | ARM_PTP_S)
#define ARM_MMU_USCN_RW     (ARM_PTP_SC | ARM_PTP_AP_URW | ARM_PTP_S)
#define ARM_MMU_KSCN_RO     (ARM_PTP_SC | ARM_PTP_AP_KRO | ARM_PTP_S)
#define ARM_MMU_KSCN_RW     (ARM_PTP_SC | ARM_PTP_AP_KRW | ARM_PTP_S)
#define ARM_MMU_KSCN_CB     (ARM_PTP_WB | ARM_PTP_TEX(1))

#define ARM_MMU_CR_SET      (ARM_MMU_CR_XP|ARM_MMU_CR_X|ARM_MMU_CR_I|ARM_MMU_CR_Z|ARM_MMU_CR_F|ARM_MMU_CR_M)
//#define ARM_MMU_CR_SET      (ARM_MMU_CR_XP|ARM_MMU_CR_I|ARM_MMU_CR_Z|ARM_MMU_CR_F|ARM_MMU_CR_M)
#define ARM_MMU_CR_CLR      (ARM_MMU_CR_EE)
#define ARM_MMU_TTB_ATTR    (ARM_TTBR_IRGN_WA | ARM_TTBR_RGN_WA | ARM_TTBR_NOS | ARM_TTBR_S)
#define ARM_MMU_PTE_ATTR    (ARM_PTE_SP_XN | ARM_MMU_PTE_C)

typedef struct {
	uint32_t upte_ro;		// user RO mappings
	uint32_t upte_rw;		// user RW mappings
	uint32_t kpte_ro;		// kern RO mappings
	uint32_t kpte_rw;		// kern RW mappings
	uint32_t mask_nc;		// bits to clear for uncached mappings
	uint32_t l1_pgtable;	// bits to set for L1 page table entry
	uint32_t uscn_ro;		// user RO section mapping
	uint32_t uscn_rw;		// user RW section mapping
	uint32_t kscn_ro;		// kern RO section mapping
	uint32_t kscn_rw;		// kern RW section mapping
	uint32_t kscn_cb;		// cacheable section mapping
	uint32_t cr_set;		// CP15 C0 values to set
	uint32_t cr_clr;		// CP15 C0 values to clear
	uint32_t ttb_attr;
	uint32_t pte_attr;
} arm_mmu_attr_t;

#endif
