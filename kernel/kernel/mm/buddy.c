#include <libc/include/string.h>
#include <platform/include/bitops.h>
#include <boot/include/bootinfo.h>
#include "kernel.h"
#include "cpu_common.h"
#include "buddy.h"


page_t *g_pages;
uint32_t g_mmdebug;
extern uint32_t slob_pages_count;

struct {
    page_t *free_blocks[PAGE_MAX_ORDER];
    uint32_t pages_max_count;
    uint32_t total_pages_count;
    uint32_t usable_pages_count;
    uint32_t pages_free_count;
    uint32_t shared_pages_count;
    struct spinlock lock;
} kmem_blocks;

#ifdef ARCH_MAP_IO
typedef arch_mem_space_t    kmem_space_t;
#else
typedef struct
{
    paddr_t base;
    paddr_t top;
} kmem_space_t;
#endif
static kmem_space_t rom_space[2];   // for kef image
static kmem_space_t io_space[8];
static kmem_space_t ram_space[6];
static kmem_space_t usable_space[4];

static bool check_space_addr(paddr_t paddr, const kmem_space_t* space, size_t size)
{
    if (NULL_PADDR == paddr)
        return false;
    for (size_t i = 0; i < size; i++)
    {
        if (paddr >= space[i].base && paddr < space[i].top)
            return true;
    }
    return false;
}

static void init_page_block(page_t *head, uint8_t order, int type)
{
    head->order = order;
    for (int i = 1; i < (1 << order); i++)
    {
        page_t *page = head + i;

        page->type = type;
        page->order = order;
        page->ref_cnt = head->ref_cnt;
    }
}

static inline bool block_is_mergable(page_t *page, uint8_t order)
{
    return (page->type == PT_FREE) && (order == page->order);
}

static inline page_t *first_free_pages(uint8_t order)
{
    return kmem_blocks.free_blocks[order];
}
#define FREE_PAGES_FOREACH(p, o)    \
    for (page_t *p = first_free_pages(o); p; p = p->next)

static void free_pages_push(page_t *page, uint8_t order)
{
    //if (kmem_blocks.usable_pages_count)
    //    KDBG("Push page: 0x%x in order %u\n", page_to_pn(page), order);
    page->type = PT_FREE;
    // page->ref_cnt = 0;
    init_page_block(page, order, PT_FREE);
    page_push(&kmem_blocks.free_blocks[order], page);
    kmem_blocks.pages_free_count += (1 << order);
}

static page_t *free_pages_pop(uint8_t index, uint8_t order)
{
    page_t *page = page_pop(&kmem_blocks.free_blocks[index]);

    //KDBG("Pop page: 0x%x in order %u\n", page_to_pn(page), order);
    if (page)
    {
        page->type = PT_HEAD;
        page->ref_cnt = 1;
        init_page_block(page, order, PT_ELEM);
        kmem_blocks.pages_free_count -= (1 << index);
    }
    return page;
}

static void free_pages_remove(page_t *page)
{
    uint8_t order = page->order;

    //KDBG("Remove page: 0x%x in order %u\n", page_to_pn(page), order);
    if (page_remove(&kmem_blocks.free_blocks[order], page))
    {
        page->type = PT_HEAD;
        page->ref_cnt = 1;
        init_page_block(page, order, PT_ELEM);
        kmem_blocks.pages_free_count -= (1 << order);
    }
}

static uint32_t free_pages_count(page_t *page)
{
    return pages_count(page);
}

static uint32_t inline free_pages_in_block(uint8_t order)
{
    return pages_count(kmem_blocks.free_blocks[order]);
}

static void add_free_pages(size_t start_page, size_t end_page)
{
    while (start_page <= end_page)
    {
        uint8_t order = ffs(start_page);

        if (order >= PAGE_MAX_ORDER)
        {
            order = PAGE_MAX_ORDER - 1;
        }
        while (end_page - start_page + 1 < (1 << order))
        {
            order -= 1;
        }
        free_pages_push(&g_pages[start_page], order);
        start_page += (1 << order);
    }
}

static void add_free_area(paddr_t start_addr, paddr_t end_addr)
{
    size_t start_page = paddr_to_pn(start_addr);
    size_t end_page = paddr_to_pn(end_addr);

    add_free_pages(start_page, end_page);
}

static void put_free_block(page_t *page)
{
    uint8_t order = page->order;

    page->type = PT_FREE;
    while (order + 1 < PAGE_MAX_ORDER)
    {
        uint32_t pn = page_to_pn(page) ^ (1L << order);
        
        if (pn >= kmem_blocks.pages_max_count)
        {
            break;
        }

        page_t *buddy = pn_to_page(pn);

        if (block_is_mergable(buddy, order))
        {
            free_pages_remove(buddy);
            if (buddy < page)
            {
                page_t *p = page;

                page = buddy;
                buddy = p;
            }
            order++;
        }
        else
        {
            break;
        }
    }
    free_pages_push(page, order);
}

static page_t *find_free_blocks(uint32_t count, uint8_t order)
{
    for (int i = (int)order; i >= 0; i--)
    {
        uint32_t step = 1L << order;

        if (free_pages_in_block(i) * step < (size_t)count)
        {
            continue;
        }

        FREE_PAGES_FOREACH(page, order)
        {
            if (free_pages_count(page) * step < count)
            {
                break;
            }

            if (page_to_pn(page) + count >= kmem_blocks.pages_max_count)
            {
                continue;
            }

            uint32_t find_pages = step;

            for (page_t *p = page + step; block_is_mergable(p, order); p += step)
            {
                find_pages += step;
            }
            if (find_pages >= count)
            {
                return page;
            }
        }
    }

    return NULL;
}

static void space_info(const kmem_space_t* space, int size)
{
    for (int i = 0; i < size && space->top; i++)
    {
        KINFO(" %p ~ %p\n", space->base, space->top);
        space++;
    }
}

static void space_init(const bsp_bootinfo_t* p_bootinfo)
{
    rom_space[0].base = PA_DOWN((paddr_t)p_bootinfo->mods_hdr_ptr->prog_hdr[0].lma);
    rom_space[0].top = PA_UP(p_bootinfo->load_end_addr);
#ifdef ARCH_MAP_IO
    arch_get_io_space(&io_space[0], ARRAY_SIZE(io_space));
#else
    int io_num = 0;
    const memmap_entry_t* memmap = p_bootinfo->memmap;
    uint16_t memmap_number = p_bootinfo->memmap_number;

    for (uint16_t i = 0; i < memmap_number; i++)
    {
        if (MEMMAP_IO_ZONE != memmap[i].type && MEMMAP_RESERVED != memmap[i].type)
            continue;
        io_space[io_num].base = memmap[i].base;
        io_space[io_num].top = memmap[i].base + memmap[i].length;
        io_num++;
    }
#endif
}

void mem_init(syspage_t* p_syspage)
{
extern void kmem_cache_init(void);
extern paddr_t early_calloc(size_t size, unsigned align);

    int ram_segs = 0;
    int usable_segs = 0;
    size_t pages_size;
    paddr_t top_addr = 0LL;

    arch_mm_init(p_syspage);

    //Initialize kmem_blocks struct
    memset(&kmem_blocks, 0, sizeof(kmem_blocks));
    spinlock_init(&kmem_blocks.lock);

    memset(&rom_space, 0, sizeof(rom_space));
    memset(&ram_space, 0, sizeof(ram_space));
    memset(&io_space, 0, sizeof(io_space));

    const bsp_bootinfo_t* p_bootinfo = get_bsp_bootinfo();
    const memmap_entry_t* memmap = p_bootinfo->memmap;

    // First, calculate how big the page array needs to be.
    for (uint16_t i = 0; i < p_bootinfo->memmap_number; i++)
    {
        paddr_t base = memmap[i].base;
        paddr_t top = base + memmap[i].length;
   
        if(top > top_addr)
        {
            top_addr = top;
        }
        // KDBG("memmap%d: %p ~ %p, %x\n", i, base, top, memmap[i].type);
        if (MEMMAP_KERNEL_AND_MODULES == memmap[i].type
            || MEMMAP_USABLE == memmap[i].type)
        {
            kmem_blocks.total_pages_count += PFN_UP(memmap[i].length);
            if (usable_segs < ARRAY_SIZE(ram_space))
            {
                if (ram_segs && base == ram_space[ram_segs-1].top)
                    ram_space[ram_segs-1].top = top;
                else
                {
                    ram_space[ram_segs].base = base;
                    ram_space[ram_segs].top = top;
                    ram_segs++;
                }
            }
        }
    }
    kmem_blocks.pages_max_count = top_addr >> PAGE_SHIFT;
    KDBG("top_addr: %p, pages_total: %x\n", top_addr, kmem_blocks.pages_max_count);
    pages_size = sizeof(struct page) * kmem_blocks.pages_max_count;

    // Second, find a location with enough free g_pages to host the page array.
    paddr_t free_addr = early_calloc(pages_size, PAGE_SIZE);
    // From now on,early_calloc will not be uesd any more.

    g_pages = (page_t*)paddr2ptr(free_addr);
    // Initialise entire pages to 0
    memset(g_pages, 0, pages_size);
    free_addr = PA_UP(p_syspage->meminfo.alloc_top_paddr);
    if (free_addr < p_bootinfo->load_end_addr)
        free_addr = PA_UP(p_bootinfo->load_end_addr);
    KDBG("g_pages: %p + %x, free_addr: %p\n", g_pages, pages_size, free_addr);

    p_region_t* p_region = p_syspage->meminfo.mem_p_regs.list;
    uint8_t reg_cnt = p_syspage->meminfo.mem_p_regs.count;
    // Third, populate free bitmap entries according to ramlist.
    for (uint8_t i = 0; i < reg_cnt; i++)
    {
        paddr_t start_addr = p_region[i].start;
        paddr_t end_addr = p_region[i].end;
        size_t size = end_addr - start_addr;

        //Only address above bitmap_used_space_end are marked as usable!
        KDBG("RAM: %p ~ %p\n", start_addr, end_addr);
        if (start_addr < free_addr && free_addr < end_addr)
        {
            size -= free_addr - start_addr;
            start_addr = free_addr;
        }
        if (!size)
        {
            continue;
        }
        // KDBG("add_free_area: %p ~ %p\n", start_addr, end_addr);
        add_free_area(start_addr, end_addr - 1);
        if (usable_segs < ARRAY_SIZE(ram_space))
        {
            usable_space[usable_segs].base = start_addr;
            usable_space[usable_segs].top = end_addr;
            usable_segs++;
        }
    }
    kmem_blocks.usable_pages_count = kmem_blocks.pages_free_count;
    g_mmdebug = 0;
    kmem_cache_init();
    space_init(p_bootinfo);
    mem_info();
}

void mem_info(void)
{
    KINFO("----------------------------------------------------------------\n");
    for (uint32_t i = 0; i < PAGE_MAX_ORDER; i++)
    {
        uint32_t free_count = free_pages_in_block(i);

        if (!free_count)
        {
            continue;
        }
        KINFO("O%u:\t", i);
        if (free_count < 8)
        {
            FREE_PAGES_FOREACH (page, i)
            {
                KINFO(" %x", page_to_pn(page));
            }
        }
        else
        {
            KINFO(" %u pages", free_count);
        }
        KINFO("\n");
    }
    KINFO("Total pages 0x%x, usable pages 0x%x, free pages 0x%x\n used pages 0x%x, shared pages 0x%x, slob pages 0x%x\n",
        kmem_blocks.usable_pages_count,
        kmem_blocks.total_pages_count,
        kmem_blocks.pages_free_count,
        kmem_blocks.usable_pages_count - kmem_blocks.pages_free_count,
        kmem_blocks.shared_pages_count,
        slob_pages_count);
    KINFO("ROM space:\n");
    space_info(rom_space, ARRAY_SIZE(rom_space));
    KINFO("IO space:\n");
    space_info(io_space, ARRAY_SIZE(io_space));
    KINFO("RAM space:\n");
    space_info(ram_space, ARRAY_SIZE(ram_space));
    KINFO("Usable RAM space:\n");
    space_info(usable_space, ARRAY_SIZE(usable_space));
    KINFO("Kernel PTE: %p\n", (paddr_t)g_syspage_ptr->pgdir[0]);
    KINFO("----------------------------------------------------------------\n");
}

void get_meminfo(mem_info_t* p_meminfo)
{
    if (!p_meminfo)
        return;
    p_meminfo->total_pages = kmem_blocks.total_pages_count;
    p_meminfo->free_pages = kmem_blocks.pages_free_count;
    p_meminfo->shared_pages = kmem_blocks.shared_pages_count;
    p_meminfo->slob_pages = slob_pages_count;
}

bool is_rom_paddr(paddr_t paddr)
{
    return check_space_addr(paddr, rom_space, ARRAY_SIZE(rom_space));
}

bool is_io_paddr(paddr_t paddr)
{
    return check_space_addr(paddr, io_space, ARRAY_SIZE(io_space));
}

bool is_ram_paddr(paddr_t paddr)
{
    return check_space_addr(paddr, ram_space, ARRAY_SIZE(ram_space));
}

bool is_usable_paddr(paddr_t paddr)
{
    return check_space_addr(paddr, usable_space, ARRAY_SIZE(usable_space));
}

paddr_t mem_max_paddr(void)
{
    return pn_to_paddr(kmem_blocks.pages_max_count) - 1;
}

page_t *alloc_page_block(uint8_t order)
{
    if (order >= PAGE_MAX_ORDER)
    {
        return NULL;
    }

    page_t *page = NULL;

    lock_acq(&kmem_blocks.lock);
    for (uint32_t i = order; i < PAGE_MAX_ORDER; i++)
    {
        page = free_pages_pop(i, order);

        if (!page)
        {
            continue;
        }

        if (order < i)
        {
            size_t left_start_page = page_to_pn(page) + (1 << order);
            size_t left_end_page = page_to_pn(page) + (1 << i) - 1;

            add_free_pages(left_start_page, left_end_page);
        }
        break;
    }
    lock_rel(&kmem_blocks.lock);

    return page;
}

uint32_t free_page_block(page_t *page)
{
    uint8_t order = page->order;
    uint32_t page_count = 1 << order;

    ASSERT(page->type == PT_HEAD || page->type == PT_SLOB);

    if (order >= PAGE_MAX_ORDER)
    {
        WARN("Invalid order%u in page %u\n", order, page_to_pn(page));
        return 0;
    }
    if (!--page->ref_cnt)    // maybe atomic
    {
        lock_acq(&kmem_blocks.lock);
        put_free_block(page);
        lock_rel(&kmem_blocks.lock);
    }

    return page_count;
}

paddr_t alloc_pages(uint32_t count, uint32_t flags)
{
    uint32_t pages = 0;
    page_t *page = NULL;

    if (!count)
    {
        return NULL_PADDR;
    }

    if (!kmem_blocks.pages_free_count)  // before mem_init
    {
        extern paddr_t early_calloc(size_t size, unsigned align);
        return early_calloc(count << PAGE_SHIFT, PAGE_SIZE);
    }

    if (count <= ALLOC_MAX_PAGES)
    {
        uint8_t order = get_order(count);

        page = alloc_page_block(order);
        pages = 1L << order;
    }
    else
    {
        lock_acq(&kmem_blocks.lock);
        page = find_free_blocks(count, PAGE_MAX_ORDER - 1);
        if (page)
        {
            uint8_t order = page->order;
            uint32_t blocks = 0;
            uint32_t step = 1L << order;
            page_t *p = page, *prev = NULL;

            for (pages = 0; pages < count; p += step, pages += step)
            {
                free_pages_remove(p);
                p->order = order;
                if (prev)
                {
                    prev->next = p;
                }
                prev = p;
                blocks++;
            }
            //p->next = NULL;
        }
        lock_rel(&kmem_blocks.lock);
    }

    if (page)
    {
        //KDBG("Alloc %u pages => %u pages at: 0x%x\n", count, pages, page_to_pn(page));
        paddr_t paddr = page_to_paddr(page);

        if (NULL_PADDR != paddr && (flags & MEM_FLAGS_ZERO))
        {
            void *ptr = paddr2ptr(paddr);

            if (ptr)
                memset(ptr, 0, count * PAGE_SIZE);
        }

        return paddr;
    }
    else
    {
        WARN("Alloc 0x%x pages failed!\n", count);
    }

    return NULL_PADDR;
}

uint32_t reference_pages(paddr_t paddr)
{
    page_t *page = paddr_to_page(paddr);

    if (page_to_pn(page) >= kmem_blocks.pages_max_count)
    {
        WARN("paddr: %p overflow!", paddr);
        return 0;
    }

    if (page->type != PT_HEAD && page->type != PT_SLOB)
    {
        // WARN("Invalid page type: %x", page->type);
        return 0;
    }

    uint32_t ref_cnt = 0;
    //	uint32_t step = PAGES_COUNT(page);

    lock_acq(&kmem_blocks.lock);
    do
    {
        page_t *next = page->next;
        if (!ref_cnt)
            ref_cnt = (uint32_t)page->ref_cnt;
        if (1 == ref_cnt)
            kmem_blocks.shared_pages_count += PAGES_COUNT(page);
        page->ref_cnt++;
        // KDBG("reference_pages: page:0x%x cnt:%d\n", page_to_pn(page), page->ref_cnt);
        page = next;
    } while (page);
    lock_rel(&kmem_blocks.lock);

    return ref_cnt;
}

error_t free_pages(paddr_t paddr)
{
    page_t *page = paddr_to_page(paddr);

    if (page_to_pn(page) >= kmem_blocks.pages_max_count)
    {
        WARN("paddr: %p overflow!", paddr);
        return ERR_INVALID_ARG;
    }

    if (page->type != PT_HEAD && page->type != PT_SLOB)
    {
        WARN("Invalid page type: %x", page->type);
        return ERR_UNAVAILABLE;
    }

    //	uint32_t step = PAGES_COUNT(page);

    lock_acq(&kmem_blocks.lock);
    do
    {
        page_t *next = page->next;
        if (!--page->ref_cnt)
        {
            put_free_block(page);
        }
        else if (1 == page->ref_cnt)
            kmem_blocks.shared_pages_count -= PAGES_COUNT(page);
        // KDBG("free_pages: page:0x%x cnt:%d\n", page_to_pn(page), page->ref_cnt);
        page = next;
    } while (page);
    lock_rel(&kmem_blocks.lock);

    return EOK;
}

uint32_t count_page_block(page_t *page)
{
    uint32_t pages = 0;
    uint32_t step = PAGES_COUNT(page);

    if (PT_HEAD == page->type)
    {
        lock_acq(&kmem_blocks.lock);
        do
        {
            pages += step;
        } while ((page = page->next) != NULL);
        lock_rel(&kmem_blocks.lock);
    }
    else
    {
        pages = step;
    }

    return pages;
}

void page_info(const page_t *page)
{
    ASSERT(page);

    KINFO("page:%x, ptr:%p, paddr:%p\n", page_to_pn(page), page, page_to_paddr(page));
    KINFO(" type=%u, order=%u, ref_cnt=%u",
        page->type, page->order, page->ref_cnt);
    if (PT_SLOB == page->type)
    {
        KINFO(" cache_idx=%u, free_count=%u, free_offs=%x\n",
            page->cache_idx, page->free_count, page->free_offs);
    }
    else
        KINFO("\n");
    if (page->next)
    {
        KINFO(" next page =>");
        page_info(page->next);
    }
}

uint32_t paddr_pages(paddr_t paddr)
{
    page_t *page = paddr_to_page(paddr);

    if (!page)
        return 0;
    return count_page_block(page);
}