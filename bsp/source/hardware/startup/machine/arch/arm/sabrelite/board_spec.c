#include <libc/include/stdio.h>
#include <boot/include/bootinfo.h>

static void mmu_init(syspage_t* p_syspage)
{
    arm_mmu_attr_t* mmu_attr = &p_syspage->loadinfo->mmu_attr;

    mmu_attr->upte_ro = ARM_MMU_UPTE_RO;
    mmu_attr->upte_rw = ARM_MMU_UPTE_RW;
    mmu_attr->kpte_ro = ARM_MMU_KPTE_RO;
    mmu_attr->kpte_rw = ARM_MMU_KPTE_RW;
    mmu_attr->mask_nc = ARM_MMU_MASK_NC;
    mmu_attr->l1_pgtable = ARM_MMU_L1_PGTABLE;
    mmu_attr->uscn_ro = ARM_MMU_USCN_RO;
    mmu_attr->uscn_rw = ARM_MMU_USCN_RW;
    mmu_attr->kscn_ro = ARM_MMU_KSCN_RO;
    mmu_attr->kscn_rw = ARM_MMU_KSCN_RW;
    mmu_attr->kscn_cb = ARM_MMU_KSCN_CB;
    mmu_attr->cr_set = ARM_MMU_CR_SET;
    mmu_attr->cr_clr = ARM_MMU_CR_CLR;
    mmu_attr->ttb_attr = ARM_MMU_TTB_ATTR;
    mmu_attr->pte_attr = ARM_MMU_PTE_ATTR;
}

extern void debug_init(bsp_device_t*);
void board_init(syspage_t* p_syspage)
{
    /* 
     * put any board specific init (such as checking for arguments 
     * passed from IPL here.
     */

    bsp_add_device(p_syspage);
    debug_init(p_syspage->devices[p_syspage->num_dev-1]);
    p_syspage->size += bsp_device_size(p_syspage->devices[p_syspage->num_dev-1]->num_intf);
    mmu_init(p_syspage);
}
