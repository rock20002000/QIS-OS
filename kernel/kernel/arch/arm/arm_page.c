#include "arch.h"
#include "interrupt.h"
#include "kernel.h"
#include "mm/buddy.h"
#include <mmu_arm32.h>
#include "interrupt.h"
#include <platform/include/dump.h>

#if (ARM_MMU_PTE_KU == ARM_MMU_PTE_CB)
 #define REMOVE_PTE_CB
#endif

extern PROCESS_t g_ker_process;
#define kernel_mmu  (g_ker_process.arch.mmu)

paddr_t kernel_paddr_start, kernel_paddr_end;
size_t kernel_p2v_offset;

static bsp_bootinfo_t bsp_module;
static arm_mmu_attr_t* mmu_attr;
static vaddr_t iomap_start, iomap_end;

vaddr_t g_gicvaddr = NULL_VADDR;
vaddr_t g_gptvaddr = NULL_VADDR;

extern void arm_flush_idtlb(void);

static inline bool arch_paddr_exist(paddr_t paddr)
{
    return (paddr & ARM_PTE_MASK);
}

static inline bool arch_pte_exist(const paddr_t* pte)
{
    return arch_paddr_exist(*pte);
}

static inline bool arch_pte_sp(paddr_t pte)
{
    return ARM_PTE_SP == (ARM_PTE_SP & pte);
}

static inline paddr_t get_attr(uint32_t flags)
{
    paddr_t attrs;

    if (flags & MAP_KERNEL)
    {
      attrs = (flags & PROT_WRITE) ? mmu_attr->kpte_rw : mmu_attr->kpte_ro;
    }
    else
    {
      attrs = (flags & PROT_WRITE) ? mmu_attr->upte_rw : mmu_attr->upte_ro;
    }
    if (flags & PROT_NOCACHE)
        attrs &= ~mmu_attr->mask_nc;
    // else
    //     attrs |= ARM_PTE_CB|ARM_PTE_SP_TEX_MASK;
    if (!(flags & PROT_EXEC))
        attrs |= ARM_PTE_SP_XN;
    return attrs;
}

static paddr_t* get_entry(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t* p_mask)
{
  paddr_t* p_entry = NULL;
  int l1_entry = vaddr >> ARM_SC_SHIFT;
  paddr_t l1_type = ARM_PTP_MASK & mmu->pde[l1_entry];

  if (l1_type == ARM_PTP_L2)
  {
    int l2_entry = (vaddr & ARM_SCMASK) >> PAGE_SHIFT;
    paddr_t* pte = (paddr_t*)paddr2ptr(mmu->pde[l1_entry] & ~ARM_L2_MASK);

    if (arch_pte_sp(pte[l2_entry]))
    {
      *p_mask = PAGE_MASK;
      p_entry = &pte[l2_entry];
    }
  }
  else if (l1_type == ARM_PTP_SC)
  {
    p_entry = &mmu->pde[l1_entry];
    *p_mask =ARM_SCMASK;
  }

  return p_entry;
}
#ifdef REMOVE_PTE_CB
static error_t arch_pte_set_nc(struct arch_mmu* mmu, vaddr_t vaddr)
{
  paddr_t mask = 0;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry)
    return ERR_NOT_FOUND;

  if (mask == PAGE_MASK)
  {
    *p_entry &= ~mmu_attr->mask_nc;
    arm_flush_idtlb_addr((vaddr_t)p_entry);
  }
  else if (mask == ARM_SCMASK)
  {
    *p_entry &= ~ARM_PTP_CB;
    arm_flush_idtlb_addr((vaddr_t)p_entry);
  }
  else
    KWARN("Unkown L1 type: %x", paddr);
  return EOK;
}
#endif
static bool map_pde(paddr_t* pde)
{
  paddr_t pte_addr = alloc_pages(1, MEM_FLAGS_ZERO | PMEM);

  if (pte_addr == NULL_PADDR) 
      return false;
  arm_flush_idtlb_addr((vaddr_t)paddr2kaddr(pte_addr));
#ifdef REMOVE_PTE_CB
  arch_pte_set_nc(&kernel_mmu, paddr2kaddr(pte_addr));
#endif
  pte_addr |= mmu_attr->l1_pgtable;
  for (int i = 0; i < PAGE_SIZE/ARM_L2_SIZE; i++)
  {
    pde[i] = pte_addr;
    pte_addr += ARM_L2_SIZE;
    arm_flush_idtlb_addr((vaddr_t)&pde[i]);
  }
  return true;
}
static paddr_t* map_entry(struct arch_mmu* mmu, vaddr_t vaddr)
{
  int l1_entry = vaddr >> ARM_SC_SHIFT;

  if ((ARM_PTP_MASK & mmu->pde[l1_entry]) != ARM_PTP_L2)
  {
    int l1_base = l1_entry & ~((PAGE_SIZE/ARM_L2_SIZE) - 1);

    if (!map_pde(&mmu->pde[l1_base])) 
    {
        WARN("alloc failed for page map at vaddr:%p", vaddr);
        return NULL;
    }
  }

  int l2_entry = (vaddr & ARM_SCMASK) >> PAGE_SHIFT;

  return (paddr_t*)paddr2ptr(mmu->pde[l1_entry] & ~ARM_L2_MASK) + l2_entry;
}
#if 1
//	static
vaddr_t arch_map_iospace(paddr_t paddr, size_t size, bool user)
{
  size_t offs = paddr & PAGE_MASK;

  if (iomap_start + offs + size <= iomap_end)
  {
    vaddr_t vaddr = iomap_start + offs;

    if (EOK == arch_map_area(&kernel_mmu, vaddr, paddr, size, user? MAP_FLAGS_IO : KERNEL_MAP_IOFLAGS))
    {
      iomap_start += PA_UP(offs + size);
      return vaddr;
    }
  }
  return NULL_VADDR;
}
#else
vaddr_t arch_map_iospace(paddr_t paddr, size_t size)
{
  size_t offs = paddr & ARM_SCMASK;
  vaddr_t vaddr = iomap_start + offs;

  size += offs;
  if (iomap_start + size > iomap_end)
    return NULL_VADDR;

  paddr = (paddr - offs) | (mmu_attr->kscn_rw | ARM_PTP_XN);

  paddr_t* pde = kernel_mmu.pde + (vaddr >> ARM_SC_SHIFT);

  for (size_t ss = 0; ss < size; ss += ARM_SCSIZE)
  {
    *pde = paddr;
    arm_flush_idtlb_addr((vaddr_t)pde);
    pde++;
    paddr += ARM_SCSIZE;
  }
  iomap_start = ALIGN_UP(iomap_start + size, ARM_SCSIZE);

  return vaddr;
}
#endif
static void arch_unmap_bsp(vaddr_t start, vaddr_t end)
{
  paddr_t* pde = kernel_mmu.pde + (start >> ARM_SC_SHIFT);

  lock_acq(&kernel_mmu.lock);
  for (; start < end; start += ARM_SCSIZE)
  {
    if (ARM_PTP_MASK & *pde)
    {
      arm_flush_idtlb_addr(start);
      *pde &= ~ARM_L2_MASK;
      arm_flush_idtlb_addr((vaddr_t)pde);
    }
    pde++;
  }
  // arm_flush_idtlb();
  lock_rel(&kernel_mmu.lock);
}

#ifdef USER_INCLUDE_KERNEL_PTE
static void arch_alloc_pde(struct arch_mmu* mmu, vaddr_t vstart, vaddr_t vend)
{
  for (int i = (vstart >> ARM_SC_SHIFT); i <= (vend >> ARM_SC_SHIFT); i++)
  {
    if (!mmu->pde[i])
    {
      map_pde(&mmu->pde[i]);
      // KDBG("arch_alloc_pde(%d): %p\n", i, i << ARM_SC_SHIFT);
    }
  }
}
#endif

const bsp_bootinfo_t* get_bsp_bootinfo(void)
{
    return &bsp_module;
}

void arch_syspage_init(struct syspage** pp_syspage)
{
  paddr_t ram_end = 0;
  syspage_t* p_syspage = *pp_syspage;
  p_region_t* p_region = p_syspage->meminfo.mem_p_regs.list;
  uint8_t reg_cnt = p_syspage->meminfo.mem_p_regs.count;

  for (uint8_t i = 0; i < reg_cnt; i++)
  {
    if (p_region[i].end > ram_end)
      ram_end = p_region[i].end;
  }

  kernel_p2v_offset = p_syspage->loadinfo->ker_hdr.prog_hdr[0].vma - p_syspage->loadinfo->ker_hdr.prog_hdr[0].lma;
  kernel_paddr_start = p_syspage->loadinfo->load_start_addr;
  kernel_paddr_end = kernel_paddr_start + p_syspage->loadinfo->ram_map_size - 1;

  p_syspage->meminfo.kvaddr_offs = kernel_p2v_offset;
  p_syspage->meminfo.avoid_list = (struct avoid_entry*)paddr2ptr((paddr_t)p_syspage->meminfo.avoid_list);
  p_syspage->meminfo.ram_list = (struct ram_entry*)paddr2ptr((paddr_t)p_syspage->meminfo.ram_list);
  p_syspage->early_alloc_func = (paddr_t (*)(meminfo_entry_t*, size_t, size_t))paddr2ptr((paddr_t)p_syspage->early_alloc_func);

  for (int i = 0; i < 4; i++)
  {
    p_syspage->intrinfo[i].id = (int (*)(void*))paddr2ptr((paddr_t)p_syspage->intrinfo[i].id);
    p_syspage->intrinfo[i].eoi = (void (*)(void*))paddr2ptr((paddr_t)p_syspage->intrinfo[i].eoi);
    p_syspage->intrinfo[i].mask = (int (*)(void*))paddr2ptr((paddr_t)p_syspage->intrinfo[i].mask);
    p_syspage->intrinfo[i].unmask = (int (*)(void*))paddr2ptr((paddr_t)p_syspage->intrinfo[i].unmask);
  }

  mmu_attr = (arm_mmu_attr_t*)paddr2ptr((paddr_t)&(p_syspage->loadinfo->mmu_attr));

  iomap_start = p_syspage->loadinfo->io_start_vaddr;
  iomap_end = iomap_start + p_syspage->loadinfo->io_map_size - 1;
#ifdef USER_INCLUDE_KERNEL_PTE
  vaddr_t io_start_vaddr = iomap_start;
#endif
  arch_pte_init(&kernel_mmu, p_syspage->pgdir[get_cpuvar()->cpu_id]);

  paddr_t map_pstart = kernel_paddr_end + 1;
  vaddr_t map_vstart = map_pstart + kernel_p2v_offset;
  vaddr_t map_vend = (KERNEL_END_VADDR + 1) > ram_end ? (KERNEL_END_VADDR + 1) : ram_end;

  if (map_vend > map_vstart)
  {
    KINFO("Kernel map space: %p ~ %p\n", map_vstart, map_vend);
    arch_map_area(&kernel_mmu, map_vstart, map_pstart, map_vend - map_vstart, KERNEL_MAP_FLAGS);
    kernel_paddr_end = map_vend - kernel_p2v_offset -1;

    for (uint8_t i = 0; i < reg_cnt; i++)
    {
      if (p_region[i].end > kernel_paddr_end)
        p_region[i].end = kernel_paddr_end;
    }
  }

  p_syspage->loadinfo = (arch_boot_info_t*)paddr2ptr((paddr_t)p_syspage->loadinfo);

  *pp_syspage = (syspage_t*)paddr2ptr((paddr_t)p_syspage);

  if (p_syspage->systime.get_cycles)
  {
    p_syspage->systime.get_cycles = (size_t (*)(const struct systime_entry*))paddr2ptr((paddr_t)p_syspage->systime.get_cycles);
  }
  if (p_syspage->systime.st_size && p_syspage->systime.st_base)
  {
    p_syspage->systime.st_base = arch_map_iospace(p_syspage->systime.st_base, p_syspage->systime.st_size, true);
  }

  for (uint8_t i = 0; i < p_syspage->num_dev; i++)
  {
    if (p_syspage->devices[i])
    {
      bsp_device_t* dev = p_syspage->devices[i];

      if (dev->io_size)
      {
        vaddr_t vaddr = arch_map_iospace(dev->io_addr, dev->io_size, false);

        if (NULL_VADDR != vaddr)
          dev->io_addr = vaddr;
      }
      for (uint8_t j = 0; j < dev->num_intf; j++)
      {
        if (dev->interface[j])
          dev->interface[j] = paddr2ptr((paddr_t)dev->interface[j]);
      }
      p_syspage->devices[i] = (bsp_device_t*)paddr2ptr((paddr_t)dev);
      if (dev->interface[DEV_INIT_INDEX])
        ((void (*)(bsp_device_t*))(dev->interface[DEV_INIT_INDEX]))(p_syspage->devices[i]);
    }
  }
  p_syspage = *pp_syspage;

#ifdef USER_INCLUDE_KERNEL_PTE
  arch_alloc_pde(&kernel_mmu, io_start_vaddr, iomap_end);
  arch_alloc_pde(&kernel_mmu, KERNEL_VMAP_START, KERNEL_MAX_VADDR);
#endif

  // KDBG("cycles_per_sec: %u, systimer freq: %u\n", p_syspage->systime.cycles_per_sec, p_syspage->systime.timer_freq);
  arch_unmap_bsp(0, KERNEL_START_VADDR);
  // KDBG("debug addr:%p, irq: %d\n", p_syspage->devices[0]->io_addr, p_syspage->devices[0]->irq_num);
  // dump_words("Uart4", p_syspage->devices[0]->io_addr + 0x80, 14, NULL_VADDR);
  // dump_words("RX4MUX1:", mux_vaddr, 1, 0x20E0938);
}


void arch_intr_init(){
	uint32_t nGicAddr = (uint32_t)get_gic_base();	//	0;
	//	__asm volatile("mrc p15, 4, %0, c15, c0, 0":"=r"(nGicAddr));
	g_gicvaddr = arch_map_iospace(nGicAddr, 0x4000, false);

	g_gptvaddr = arch_map_iospace(0x2098000, 0x1000, false);
}

void arch_pte_init(struct arch_mmu* mmu, paddr_t pde)
{
  ASSERT(mmu);
  if (mmu->pde)
    return;
  spinlock_init(&mmu->lock);
  if (NULL_PADDR == pde)
  {
      mmu->pde = paddr2ptr(alloc_pages(ARM_L1_PAGES, MEM_FLAGS_ZERO | PMEM));
#ifdef REMOVE_PTE_CB
      vaddr_t vp = (vaddr_t)mmu->pde;

      for (int i = 0; i < ARM_L1_PAGES; i++)
      {
        arch_pte_set_nc(&kernel_mmu, vp);
        vp += PAGE_SIZE;
      }
#endif
  }
  else
      mmu->pde = paddr2ptr(pde);
}

// void handle_prefetch_abort(abt_frame_t* aframe);
// void handle_data_abort(abt_frame_t* aframe);
void arch_mm_init(syspage_t* p_syspage)
{
  bsp_module.ker_hdr_ptr = &p_syspage->loadinfo->ker_hdr;
  bsp_module.mods_hdr_ptr = p_syspage->loadinfo->mods_hdr;
  bsp_module.load_end_addr = p_syspage->loadinfo->load_end_addr;
  bsp_module.module_number = p_syspage->loadinfo->module_number;
  bsp_module.memmap = p_syspage->loadinfo->memmap_data.entry;
  bsp_module.memmap_number = p_syspage->loadinfo->memmap_data.entry_counts;
  KINFO("App modules: %p ~ %p\n", (paddr_t)bsp_module.mods_hdr_ptr[0].prog_hdr[0].lma, bsp_module.load_end_addr);

	// register_interrupt_handler(3, handle_prefetch_abort);
	// register_interrupt_handler(4, handle_data_abort);
  KINFO("Kernel start paddr: %p, vaddr: %p\n", kernel_paddr_start, paddr2kaddr(kernel_paddr_start));
  // KDBG(" kpte_ro:%x, kpte_rw:%x\n", mmu_attr->kpte_ro, mmu_attr->kpte_rw);
  // KDBG(" upte_ro:%x, upte_rw:%x\n", mmu_attr->upte_ro, mmu_attr->upte_rw);
  // KDBG(" l1_pgtable:%x, pte_attr:%x\n", mmu_attr->upte_ro, mmu_attr->pte_attr);
}

error_t arch_pte_map(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t paddr,
                     uint32_t flags)
{
  paddr_t* p_entry = map_entry(mmu, vaddr);

  if (!p_entry)
      return ERR_NO_MEMORY;

  if (arch_pte_exist(p_entry))
  {
      WARN("arch_pte_map: %p => *%p=%p => %p", vaddr, p_entry, *p_entry, paddr);
      return ERR_INVALID_ARG;
  }

  *p_entry = (paddr & ~PAGE_MASK) | get_attr(flags);
  arm_flush_idtlb_addr((vaddr_t)p_entry);
  // arm_flush_idtlb_addr(vaddr);

  return EOK;
}

error_t arch_map_area(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t paddr,
                     size_t size, uint32_t flags)
{
  error_t err = EOK;
  paddr_t vend = PA_UP(vaddr + size);

  paddr = (paddr & ~PAGE_MASK) | get_attr(flags);
  lock_acq(&mmu->lock);
  while (vaddr < vend)
  {
    paddr_t* p_entry = map_entry(mmu, vaddr);    
    if (!p_entry)
    {
      err = ERR_NO_MEMORY;
      break;
    }

    if (arch_pte_exist(p_entry))
    {
        WARN("arch_map_pages: %p => *%p=%p => %p", vaddr, p_entry, *p_entry, paddr);
    }
    *p_entry = paddr;
    arm_flush_idtlb_addr((vaddr_t)p_entry);
    vaddr += PAGE_SIZE;
    paddr += PAGE_SIZE;
  }
  // arm_flush_idtlb();
  lock_rel(&mmu->lock);

  return err;
}

error_t arch_pte_unmap(struct arch_mmu* mmu, vaddr_t vaddr, paddr_t* p_paddr)
{
  paddr_t mask = 0;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry || !arch_pte_exist(p_entry))
    return ERR_NOT_FOUND;

  if (p_paddr)
  {
    if (mask == PAGE_MASK)    
      *p_paddr = PA_DOWN(*p_entry);
    else if (mask == ARM_SCMASK)
      *p_paddr = *p_entry & ~ARM_SCMASK;
  }
  arm_flush_idtlb_addr(vaddr);
  *p_entry = 0;
  arm_flush_idtlb_addr((vaddr_t)p_entry);

  return EOK;
}

error_t arch_unmap_area(struct arch_mmu* mmu, vaddr_t vaddr, size_t size)
{
  error_t err = EOK;
  paddr_t vend = PA_UP(vaddr + size);

  lock_acq(&mmu->lock);
  while (vaddr < vend)
  {
    paddr_t mask = 0;
    paddr_t* p_entry = get_entry(mmu, vaddr, &mask);
    size_t entry_size = (mask == ARM_SCMASK) ? ARM_SCSIZE : PAGE_SIZE;

    if (!p_entry || !arch_pte_exist(p_entry))
    {
      err = ERR_NOT_FOUND;
      break;
    }
    arm_flush_idtlb_addr(vaddr);
    *p_entry = 0;
    arm_flush_idtlb_addr((vaddr_t)p_entry);
    vaddr = ALIGN_DOWN(vaddr + entry_size, entry_size);
  }
  // arm_flush_idtlb();
  lock_rel(&mmu->lock);

  return err;
}

paddr_t arch_pte_get(struct arch_mmu* mmu, vaddr_t vaddr)
{
  paddr_t mask;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry)
    return NULL_PADDR;
  
  return ((*p_entry & ~mask) | (vaddr & mask));
}

void arch_pte_free(struct arch_mmu* mmu) {
  ASSERT(mmu);
  if (!mmu->pde) return;

  lock_acq(&mmu->lock);
  for (int i = 0; i < ARM_L1_SIZE/sizeof(paddr_t); i++)
  {
    int l1_type = ARM_PTP_MASK & mmu->pde[i];

    if (l1_type == ARM_PTP_L2)
    {
      paddr_t l2_page = mmu->pde[i] & ~ARM_L2_MASK;
      paddr_t* pte = (paddr_t*)paddr2ptr(l2_page);

      for (int j = 0; j < PAGE_SIZE/sizeof(paddr_t); j++)
      {
        if (arch_pte_sp(pte[j]))
        {
          paddr_t paddr = pte[j] & ~PAGE_MASK;

          if (is_usable_paddr(paddr))
              free_pages(paddr);
        }
      }
      free_pages(l2_page);
      // mmu->pde[i] = 0;
      i += 3;
    }
    else if (l1_type == ARM_PTP_SC)
    {
      paddr_t paddr = mmu->pde[i] & ~ARM_SCMASK;

      if (is_usable_paddr(paddr))
          free_pages(paddr);
      // mmu->pde[i] = 0;
    }
  }
  free_pages(ptr2paddr(mmu->pde));
  mmu->pde = NULL;
  lock_rel(&mmu->lock);
}

error_t arch_pte_set_ro(struct arch_mmu* mmu, vaddr_t vaddr)
{
  paddr_t mask = 0;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry)
    return ERR_NOT_FOUND;

  paddr_t paddr = *p_entry;

  if (mask == PAGE_MASK)
  {
    paddr_t ap = (mmu_attr->upte_rw | mmu_attr->upte_ro) & paddr;

    if (ap == mmu_attr->upte_rw)
    {
      paddr &= mmu_attr->upte_rw | mmu_attr->upte_ro;
      *p_entry = paddr | mmu_attr->upte_ro;
      arm_flush_idtlb_addr((vaddr_t)p_entry);
    }
    else
    {
      ap = (mmu_attr->kpte_rw | mmu_attr->kpte_ro) & paddr;
      if (ap == mmu_attr->kpte_rw)
      {
        paddr &= mmu_attr->kpte_rw | mmu_attr->kpte_ro;
        *p_entry = paddr | mmu_attr->kpte_ro;
        arm_flush_idtlb_addr((vaddr_t)p_entry);
      }
    }
  }
  else if (mask == ARM_SCMASK)
  {
    paddr_t ap = (mmu_attr->kscn_rw | mmu_attr->kscn_ro) & paddr;

    if (ap == mmu_attr->kscn_rw)
    {
      paddr &= mmu_attr->kscn_rw | mmu_attr->kscn_ro;
      *p_entry = paddr | mmu_attr->kscn_ro;
      arm_flush_idtlb_addr((vaddr_t)p_entry);
    }
    else
    {
      ap = (mmu_attr->uscn_rw | mmu_attr->uscn_ro) & paddr;
      if (ap == mmu_attr->uscn_rw)
      {
        paddr &= mmu_attr->uscn_rw | mmu_attr->uscn_ro;
        *p_entry = paddr | mmu_attr->uscn_ro;
        arm_flush_idtlb_addr((vaddr_t)p_entry);
      }
    }
  }
  else
    KWARN("Unkown L1 type: %x", paddr);
  return EOK;
}

error_t arch_pte_set_rw(struct arch_mmu* mmu, vaddr_t vaddr)
{
  paddr_t mask = 0;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry)
    return ERR_NOT_FOUND;
  
  paddr_t paddr = *p_entry;

  if (mask == PAGE_MASK)
  {
    paddr_t ap = (mmu_attr->upte_rw | mmu_attr->upte_ro) & paddr;

    if (ap == mmu_attr->upte_ro)
    {
      paddr &= mmu_attr->upte_rw | mmu_attr->upte_ro;
      *p_entry = paddr | mmu_attr->upte_rw;
      arm_flush_idtlb_addr((vaddr_t)p_entry);
    }
    else
    {
      ap = (mmu_attr->kpte_rw | mmu_attr->kpte_ro) & paddr;
      if (ap == mmu_attr->kpte_ro)
      {
        paddr &= mmu_attr->kpte_rw | mmu_attr->kpte_ro;
        *p_entry = paddr | mmu_attr->kpte_rw;
        arm_flush_idtlb_addr((vaddr_t)p_entry);
      }
    }
  }
  else if (mask == ARM_SCMASK)
  {
    paddr_t ap = (mmu_attr->kscn_rw | mmu_attr->kscn_ro) & paddr;

    if (ap == mmu_attr->kscn_ro)
    {
      paddr &= mmu_attr->kscn_rw | mmu_attr->kscn_ro;
      *p_entry = paddr | mmu_attr->kscn_rw;
      arm_flush_idtlb_addr((vaddr_t)p_entry);
    }
    else
    {
      ap = (mmu_attr->uscn_rw | mmu_attr->uscn_ro) & paddr;
      if (ap == mmu_attr->uscn_ro)
      {
        paddr &= mmu_attr->uscn_rw | mmu_attr->uscn_ro;
        *p_entry = paddr | mmu_attr->uscn_rw;
        arm_flush_idtlb_addr((vaddr_t)p_entry);
      }
    }
  }
  else
    KWARN("Unkown L1 type: %x", paddr);
  return EOK;
}

error_t arch_pte_set_user(struct arch_mmu* mmu, vaddr_t vaddr)
{
  paddr_t mask = 0;
  paddr_t* p_entry = get_entry(mmu, vaddr, &mask);

  if (!p_entry)
    return ERR_NOT_FOUND;
  
  paddr_t paddr = *p_entry;

  if (mask == PAGE_MASK)
  {
    paddr_t ap = (mmu_attr->kpte_rw | mmu_attr->kpte_ro) & paddr;

    paddr &= ~mmu_attr->kpte_rw | mmu_attr->kpte_ro;

    if (ap == mmu_attr->kpte_rw)
    {
      *p_entry = paddr | mmu_attr->upte_rw;
      arm_flush_idtlb_addr(vaddr);
    }
    else if (ap == mmu_attr->kpte_ro)
    {
      *p_entry = paddr | mmu_attr->upte_ro;
      arm_flush_idtlb_addr(vaddr);
    }
  }
  else if (mask == ARM_SCMASK)
  {
    paddr_t ap = (mmu_attr->kscn_rw | mmu_attr->kscn_ro) & paddr;

    paddr &= mmu_attr->kscn_rw | mmu_attr->kscn_ro;
    if (ap == mmu_attr->kscn_ro)
    {
      *p_entry = paddr | mmu_attr->uscn_ro;
      arm_flush_idtlb_addr(vaddr);
    }
    else if (ap == mmu_attr->kscn_rw)
    {
      *p_entry = paddr | mmu_attr->uscn_rw;
      arm_flush_idtlb_addr(vaddr);
    }
  }
  else
    KWARN("Unkown L1 type: %x", paddr);
  return EOK;
}

void arch_pte_copy_kernel(struct arch_mmu* mmu, const struct arch_mmu* ker_mmu)
{
#ifdef USER_INCLUDE_KERNEL_PTE
  for (int i = KERNEL_START_VADDR >> ARM_SC_SHIFT; i < ARM_L1_SIZE/sizeof(paddr_t); i++)
  {
    if (ker_mmu->pde[i])
    {
      mmu->pde[i] = ker_mmu->pde[i];
      // arm_flush_idtlb_addr((vaddr_t)&mmu->pde[i]);
      // KDBG("arch_pte_copy(%d): %p\n", i, i << ARM_SC_SHIFT);
    }
  }
#else
  int l1_index;

  for (l1_index = (KERNEL_START_VADDR >> ARM_SC_SHIFT); l1_index <= (KERNEL_END_VADDR >> ARM_SC_SHIFT); l1_index++)
  {
    if (ker_mmu->pde[l1_index])
    {
      mmu->pde[l1_index] = ker_mmu->pde[l1_index];
      // arm_flush_idtlb_addr((vaddr_t)&mmu->pde[l1_index]);
    }
  }

  l1_index = ARM_L1_SIZE/sizeof(paddr_t) - 1;
  mmu->pde[l1_index] = ker_mmu->pde[l1_index];
  // arm_flush_idtlb_addr((vaddr_t)&mmu->pde[l1_index]);
#endif
}
