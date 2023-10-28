#include "lib/startup.h"
#include <mmu_arm32.h>
#include <libc/include/string.h>
#include <platform/include/mmap.h>

#ifndef PAGE_MASK
 #define PAGE_MASK      (PAGE_SIZE - 1)
#endif

//  #define KERNEL_STACK_MAX_SIZE  (PAGE_SIZE * 2 * MAX_CPU)

#ifndef RAM_START_VADDR
 #define RAM_START_VADDR    (0xC0000000)
#endif
#ifndef RAM_END_VADDR
 #define RAM_END_VADDR      (-1UL)
#endif
#define RAM_VADDR_MAXSIZE   (RAM_END_VADDR - RAM_START_VADDR + 1)

#define RAM_START_PADDR     (ram_start)
#define RAM_ADDR_P2V(paddr) ((paddr) + ram_p2v_offset)
#define RAM_ADDR_V2P(vaddr) ((vaddr) - ram_p2v_offset)

#define MAP_START_PADDR     (image_start)
#define MAP_START_VADDR     RAM_ADDR_P2V(MAP_START_PADDR)
#define RAM_MAP_SIZE        (KERNEL_VADDRESS_END - MAP_START_VADDR)
#define RAM_MAP_MAXSIZE     (RAM_END_VADDR - RAM_START_VADDR)

#define IOREMAP_START_VADDR RAM_START_VADDR
#define IOREMAP_SIZE        (iomap_size)

#define VECTOR_MAPSIZE      (0x10000)
#ifdef VECTOR_MAPSIZE
 #define VECTOR_START_VADDR (RAM_END_VADDR - VECTOR_MAPSIZE + 1)
#endif

#define ARM_L1_MAXSIZE      (ARM_L1_SIZE * arm_cpu_nums)
#define ARM_L2_MAPSIZE      (RAM_VADDR_MAXSIZE >> (PAGE_SHIFT - 2))    // 1M
#define ARM_L2_MAXSIZE      (ARM_L2_MAPSIZE * arm_cpu_nums)

#define BSP_CODE_PADDR      ((paddr_t)__image_start_addr)
#define BSP_CODE_VADDR      RAM_ADDR_P2V(BSP_CODE_PADDR)
#define BSP_CODE_SIZE       (__ro_end_addr - __image_start_addr)
// #define BSP_DATA_PADDR      ((paddr_t)__data_start_addr)
// #define BSP_DATA_SIZE       (__boot_image_end - __data_start_addr)
#define BSP_MAX_SIZE        (__boot_image_end - __image_start_addr)

// For bootinfo_ptr
// #define EXBOOT_PADDR        (0x20000000)
#ifdef EXBOOT_PADDR
 #define EXBOOT_SIZE        ARM_SCSIZE
#endif

extern char __image_start_addr[];
extern char __ro_end_addr[];
// extern char __data_start_addr[];
extern char __boot_image_end[];

static bool mmu_start;
static int arm_cpu_nums;
static paddr_t L1_paddr;
static paddr_t ram_start = -1UL, image_start, iomap_vstart;
static size_t iomap_size = 0, ram_map_size;
static size_t ram_p2v_offset;
#define	CPU_L1_TABLE(cpu)   ((L1_paddr + ((cpu) * ARM_L1_SIZE)) & ~ARM_TTBR_ATTR_MASK)
#define vaddr_to_pde(c, va) ((paddr_t*)CPU_L1_TABLE(c) + ((va) >> ARM_SC_SHIFT))

static arm_mmu_attr_t* mmu_attr;

#define MAP_PTE         MAP_SPARE3
#define MAP_FLAGS_PTE   (MAP_PTE | PROT_READ_WRITE)

#define MAP_FLAGS_CODE  (PROT_READ | PROT_EXEC)
#define MAP_FLAGS_DATA  PROT_READ_WRITE
#define MAP_FLAGS_ALL   (PROT_READ_WRITE | PROT_EXEC)

#if 0
static inline void arm_sctlr_set(uint32_t sctlr)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c1, c0, 0" : : "r" (sctlr));
}
static inline uint32_t arm_sctlr_get(void)
{
	uint32_t sctlr;
	__asm__ __volatile__("mrc	p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
	return sctlr;
}
static inline void arm_ttbr0_set(uint32_t ttbr0)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c2, c0, 0" : : "r" (ttbr0));
}
static inline uint32_t arm_ttbr0_get(void)
{
	uint32_t ttbr0;
	__asm__ __volatile__("mrc	p15, 0, %0, c2, c0, 0" : "=r"(ttbr0));
	return ttbr0;
}
static inline void arm_tbcr_set(uint32_t ttbr0)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c2, c0, 2" : : "r" (ttbr0));
}
static inline uint32_t arm_tbcr_get(void)
{
	uint32_t tbcr;
	__asm__ __volatile__("mrc	p15, 0, %0, c2, c0, 2" : "=r"(tbcr));
	return tbcr;
}

static inline void dump_words(const char* title, uintptr_t addr, size_t words)
{
    size_t i;
    const uint32_t* p = (const uint32_t*)addr;

	if (title)
		printf("%s:\n", title);
    for (i = 0; i < words; i++)
    {
        if (!(i&7))
            printf("%x:", p);
        printf(" %x", *p++);

        if (7 == (i&7))
            printf("\n");
    }
    if (7 != (i&7))
        printf("\n");
}

static paddr_t get_paddr(vaddr_t vaddr)
{
    paddr_t* pde = vaddr_to_pde(0, vaddr);

    if ((ARM_PTP_MASK & *pde) != ARM_PTP_L2)
        return NULL_PADDR;

    paddr_t* pte = (paddr_t*)(*pde & ~ARM_L2_MASK);
    paddr_t paddr = pte[(vaddr & ARM_SCMASK) >> PAGE_SHIFT];

    if (!paddr)
        return NULL_PADDR;

    return (paddr & ~PAGE_MASK) | (vaddr & PAGE_MASK);
}
#endif
/*
 * Invalidate caches & tlb
 */
static inline void arm_flush_idtlb(void)
{
  const uint32_t zero = 0;

	__asm__ __volatile__(".align 5");
	__asm__ __volatile__("mcr p15, 0, %0, c7, c5, 0" : : "r" (zero));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c6, 0" : : "r" (zero));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0" : : "r" (zero));
	__asm__ __volatile__("isb");
	__asm__ __volatile__("dsb");
}

static inline void arm_flush_idtlb_addr(vaddr_t addr)
{
	__asm__ __volatile__("mcr	p15, 0, %0, c8, c6, 1" : : "r" (addr));
	__asm__ __volatile__("mcr	p15, 0, %0, c8, c7, 1" : : "r" (addr));
}

static inline bool valid_kernel_address(vaddr_t start, uint32_t size)
{
    return (start >= KERNEL_VADDRESS_START) && ((start + size) <= KERNEL_VADDRESS_END);
}

static inline paddr_t get_pte_attr(uint32_t flags)
{
    paddr_t attrs = (flags & PROT_WRITE) ? mmu_attr->kpte_rw : mmu_attr->kpte_ro;

    if (flags & PROT_NOCACHE)
        attrs &= ~mmu_attr->mask_nc;
    // else
    //     attrs |= ARM_PTE_CB|ARM_PTE_SP_TEX_MASK;
    if (!(flags & PROT_EXEC))
        attrs |= ARM_PTE_SP_XN;
    return attrs;
}
#define set_prot(paddr, flags)  ((paddr) | get_pte_attr(flags))

static paddr_t _alloc_top;
static struct meminfo_entry* g_meminfo;
static paddr_t mmu_alloc(size_t size, size_t align)
{
    paddr_t paddr = ALIGN_DOWN(_alloc_top - size, align);

    if (paddr < (paddr_t)__boot_image_end)
    {
        paddr = calloc_ram(g_meminfo, size, align);
    }
    else
    {
        memset((void *)(paddr + g_meminfo->kvaddr_offs), 0, size);
        _alloc_top = paddr;
    }

    return paddr;
}

static paddr_t get_pde(int cpu, vaddr_t vaddr)
{
    paddr_t* pde = vaddr_to_pde(cpu, vaddr);

    if ((ARM_PTP_MASK & *pde) != ARM_PTP_L2)
        return NULL_PADDR;

    return *pde & ~ARM_L2_MASK;
}

static void map_vaddr(vaddr_t vaddr, paddr_t paddr, size_t size, uint32_t flags)
{
    size_t offs = paddr & PAGE_MASK;
    uint32_t pages = (offs + size + PAGE_MASK) >> PAGE_SHIFT;

    paddr = set_prot(paddr - offs, flags);
    // printf("map_vaddr: vaddr=%x, paddr=%x, size=%x, flags=%x/%x\n",
    //     vaddr, paddr, size, flags, paddr & PAGE_MASK);

	for (int cpu = 0; cpu < arm_cpu_nums; cpu++)
    {
		paddr_t	pa = paddr;
        vaddr_t va = vaddr;

        for (uint32_t i = 0; i < pages; i++)
        {
            paddr_t* pde = vaddr_to_pde(cpu, va);
            paddr_t* pte = (paddr_t*)(*pde & ~ARM_L2_MASK);

            if (!pte)
            {
                int l1_entry = va >> ARM_SC_SHIFT;
                paddr_t pte0 = mmu_alloc(PAGE_SIZE, PAGE_SIZE);
                paddr_t* pde0 = pde - (l1_entry & (PAGE_SIZE/ARM_L2_SIZE - 1));

                // printf("map_vaddr: vaddr=%x, paddr=%x, pde=%p, pte=%x\n", va, pa, pde0, pte0);
                pte0 |= mmu_attr->l1_pgtable;
                for (int j = 0; j < PAGE_SIZE/ARM_L2_SIZE; j++)
                {
                    *pde0++ = pte0;
                    // arm_flush_idtlb_addr((vaddr_t)pde0);
                    pte0 += ARM_L2_SIZE;
                }
                pte = (paddr_t*)(*pde & ~ARM_L2_MASK);
            }
            else if ((ARM_PTP_MASK & *pde) != ARM_PTP_L2)
            {
                *pde = (paddr_t)pte | mmu_attr->l1_pgtable;
                // arm_flush_idtlb_addr((vaddr_t)pde);
            }

            pte[(va & ARM_SCMASK) >> PAGE_SHIFT] = pa;
            // arm_flush_idtlb_addr((vaddr_t)&pte[(va & ARM_SCMASK) >> PAGE_SHIFT]);
            pa += PAGE_SIZE;
            va += PAGE_SIZE;
        }
	}
    arm_flush_idtlb();
}

static void map_section(vaddr_t vaddr, paddr_t paddr, size_t size, uint32_t flags)
{    
    size_t offs = paddr & ARM_SCMASK;
    uint32_t pdes = ARM_SC_CNT(offs + size);

    paddr -= offs;

    for (int cpu = 0; cpu < arm_cpu_nums; cpu++)
    {
        paddr_t pa = paddr | flags;
        paddr_t* pde = vaddr_to_pde(cpu, vaddr);

        for (int i = 0; i < pdes; i++)
        {
            *pde++ = pa;
            // arm_flush_idtlb_addr((vaddr_t)(pde - 1));
            pa += ARM_SCSIZE;
        }
        arm_flush_idtlb();
    }
}

static void map_bsp_1to1(paddr_t map_1to1_end_paddr)
{
    paddr_t paddr = BSP_CODE_PADDR;
    uint32_t offs = paddr & ARM_SCMASK;
    uint32_t pdes = ARM_SC_CNT(offs + map_1to1_end_paddr - paddr);
    vaddr_t vaddr = RAM_ADDR_P2V(paddr);

    // remap pte of bsp code
    map_vaddr(vaddr, paddr, BSP_CODE_SIZE, MAP_FLAGS_CODE);
    // printf("Map BSP: %x+%x to %x\n", paddr, BSP_CODE_SIZE, vaddr);

    // 1to1 remap all bsp pde
    paddr -= offs;
    for (int cpu = 0; cpu < arm_cpu_nums; cpu++)
    {
        vaddr_t va = vaddr;
        paddr_t* pde = vaddr_to_pde(cpu, paddr);
    
        for (int i = 0; i < pdes; i++)
        {
            *pde++ = get_pde(cpu, va) | mmu_attr->l1_pgtable;
            // arm_flush_idtlb_addr((vaddr_t)(pde - 1));
            va += ARM_SCSIZE;
        }
        arm_flush_idtlb();
    }
}

static void remap_kernel_sections(vaddr_t vaddr, paddr_t paddr, size_t size, uint32_t attr)
{
#define ELF_PF_X    1
    if (attr & ELF_PF_X)
    {
        // map_vaddr(vaddr, paddr, size, MAP_FLAGS_CODE);
        map_vaddr(vaddr, paddr, size, MAP_FLAGS_ALL);
    }
}

extern void add_avoid_ram(paddr64_t start, size_t size);
void arch_init_pgdir(syspage_t* p_syspage)
{
    paddr_t kernel_pstart = p_syspage->loadinfo->ker_hdr.prog_hdr[0].lma;
    paddr_t ram_end = 0, ram_map_top = p_syspage->loadinfo->load_end_addr;

    mmu_start = false;

    image_start = p_syspage->loadinfo->load_start_addr;
    ram_p2v_offset = p_syspage->loadinfo->ker_hdr.prog_hdr[0].vma - kernel_pstart;

    iomap_size = (MAP_START_VADDR - RAM_START_VADDR) & ~ARM_SCMASK;
    iomap_vstart = IOREMAP_START_VADDR;

    _alloc_top = kernel_pstart;
    g_meminfo = &p_syspage->meminfo;

    p_syspage->loadinfo->io_start_vaddr = IOREMAP_START_VADDR;
    p_syspage->loadinfo->io_map_size = IOREMAP_SIZE;

    for (int i = 0; i < p_syspage->loadinfo->memmap_data.entry_counts; i++)
    {
        uint32_t type = (uint32_t)p_syspage->loadinfo->memmap_data.entry[i].type;
        paddr_t paddr = (paddr_t)p_syspage->loadinfo->memmap_data.entry[i].base;
        size_t size = (size_t)p_syspage->loadinfo->memmap_data.entry[i].length;

        if (type == MEMMAP_KERNEL_AND_MODULES || type == MEMMAP_USABLE)
        {
            if (ram_start > paddr)
                ram_start = paddr;
            if (paddr + size > ram_end)
                ram_end = paddr + size;
        }
    }
    // printf("ram: %x ~ %x, image: %x ~ %x, io map size: %x, syspage: %p\n",
    //     ram_start, ram_end, image_start, ram_map_top, iomap_size, p_syspage);

    arm_cpu_nums = p_syspage->num_cpu;
    if (!arm_cpu_nums)
    {
        arm_cpu_nums = 1;
    }
    L1_paddr = mmu_alloc(ARM_L1_MAXSIZE, ARM_L1_SIZE);
    // printf("arch_init_pgdir map start=%x L1=%x+%x, p2v offset: %x\n",
    //     MAP_START_PADDR, L1_paddr, ARM_L1_MAXSIZE, ram_p2v_offset);

    mmu_attr = &p_syspage->loadinfo->mmu_attr;

#ifdef VECTOR_START_VADDR
    //////////////////////////////////////////////////////////
    paddr_t* vector = (paddr_t*)mmu_alloc(__PAGESIZE, __PAGESIZE);

    map_vaddr(VECTOR_START_VADDR, (paddr_t)vector, __PAGESIZE, MAP_FLAGS_ALL);
    for (int i = 0; i < 8; i++)
    {
        vector[i] = 0xe59ff018;
    }
    //////////////////////////////////////////////////////////
#endif

    if (ram_map_top < p_syspage->meminfo.alloc_top_paddr)
    {
        ram_map_top = ALIGN_UP(p_syspage->meminfo.alloc_top_paddr, PAGE_SIZE);
    }

    ram_map_size = ALIGN_UP(ram_map_top - ram_start + ARM_L2_MAXSIZE, PAGE_SIZE);  // reserved for L2
    // ram_map_size = ALIGN_UP(ram_end - ram_start, PAGE_SIZE);
    p_syspage->loadinfo->ram_map_size = ram_map_size;

    // Map all used ram in kernel space
    map_vaddr(MAP_START_VADDR, MAP_START_PADDR, ram_map_size, MAP_FLAGS_DATA);
    // 1to1 map bsp all ram
    map_bsp_1to1(ram_map_top);

#ifdef EXBOOT_PADDR
    map_section(EXBOOT_PADDR, EXBOOT_PADDR, EXBOOT_SIZE, mmu_attr->kscn_rw | ARM_PTP_XN);
#endif
   if (p_syspage->devices[DBG_DEV_INDEX]->io_size)
    {
        size_t io_size = p_syspage->devices[DBG_DEV_INDEX]->io_size;
        paddr_t paddr = p_syspage->devices[DBG_DEV_INDEX]->io_addr;
        uint32_t io_offs = paddr & ARM_SCMASK;

        if (io_size + io_offs > IOREMAP_SIZE)
            io_size = IOREMAP_SIZE - io_offs;

        map_section(paddr, paddr, io_size, mmu_attr->uscn_rw | ARM_PTP_XN);    // for debug in userspace
        // map_section(paddr, paddr, io_size, mmu_attr->kscn_rw | ARM_PTP_XN);
    }

    /* All cpus share initial kernel page table. */
    for (int i = 0; i < arm_cpu_nums; ++i)
    {
        p_syspage->pgdir[i] = CPU_L1_TABLE(i);
        printf("pgdir[%d] = %p\n", i, CPU_L1_TABLE(i));
    }
}

extern void mmu_enable(paddr_t ttbr, uint32_t mmu_cr_clr, uint32_t mmu_cr_set);
void arch_load_pgdir(syspage_t* p_syspage, unsigned cpu_idx) 
{
    paddr_t ttbr = (paddr_t)p_syspage->pgdir[cpu_idx];

    mmu_enable(ttbr, mmu_attr->cr_clr, mmu_attr->cr_set);

    mmu_start = true;
    printf("ARM mmu enable from %x + %x to %x\n", MAP_START_PADDR, ram_map_size, MAP_START_VADDR);
}

void arch_map_kernel(arch_boot_info_t* p_bootinfo)
{
    for(int i = 0; i < p_bootinfo->ker_hdr.ph_nmbers; i++)
    {
        ASSERT(valid_kernel_address(p_bootinfo->ker_hdr.prog_hdr[i].vma, p_bootinfo->ker_hdr.prog_hdr[i].mem_size));
        
        uint32_t attr = (p_bootinfo->ker_hdr.prog_hdr[i].attr) >> 32;

        remap_kernel_sections(   p_bootinfo->ker_hdr.prog_hdr[i].vma, 
                                    p_bootinfo->ker_hdr.prog_hdr[i].lma, 
                                    p_bootinfo->ker_hdr.prog_hdr[i].mem_size,
                                    attr );
#if 0
        paddr_t paddr = (paddr_t)p_bootinfo->ker_hdr.prog_hdr[i].lma;
        vaddr_t vaddr = (vaddr_t)p_bootinfo->ker_hdr.prog_hdr[i].vma;
        size_t size = p_bootinfo->ker_hdr.prog_hdr[i].mem_size;

        printf("Maped kernel sect%d %x+%x to %x, attr=%x\n", i, paddr, size, vaddr, attr);
#endif
    }
}

vaddr_t arch_paddr2vaddr(paddr_t paddr)
{
    if (!mmu_start)
        return NULL_VADDR;
    if (paddr >= MAP_START_VADDR)
        return paddr;
    if (paddr < MAP_START_PADDR || paddr >= MAP_START_PADDR + ram_map_size)
        return NULL_VADDR;
    return RAM_ADDR_P2V(paddr);
}
