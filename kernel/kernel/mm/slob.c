#include <platform/include/bitops.h>
#include "kernel.h"
#include "buddy.h"

// #define SLOB_DEBUG
#define SLOB_RESERVED_PAGE

#ifdef __LP64__
 #define OBJ_MIN_ORDER  4   // 16
#else
 #define OBJ_MIN_ORDER  3   // 8
#endif
#define OBJ_MAX_ORDER   (PAGE_SHIFT - 1)
//#define OBJ_NUMS_ORDER  (PAGE_SHIFT - OBJ_MIN_ORDER)

#define OBJ_ORDERS      (OBJ_MAX_ORDER - OBJ_MIN_ORDER + 1)
#define FREE_OFFS_NULL  (-1)

#define page_to_ptr(pg) paddr2ptr(page_to_paddr(pg))
#define ptr_to_page(p)  paddr_to_page(ptr2paddr(p))

uint32_t slob_pages_count;

typedef struct kmem_cache
{
    page_t          *alloc;
    page_t          *full;
#ifdef SLOB_RESERVED_PAGE
    page_t          *empty;
#endif
    struct spinlock lock;
    uint8_t         order;
    uint8_t			szReserver[3];
} kmem_cache_t;

static kmem_cache_t* kmem_cache[OBJ_ORDERS];
static struct spinlock kmem_cache_lock;

static kmem_cache_t kmem_cache_this;

static void *alloc_cache_page(uint8_t order)
{
    page_t *page = NULL;
#ifdef OBJ_NUMS_ORDER
    uint8_t page_order = order + OBJ_NUMS_ORDER - PAGE_SHIFT;
#else
    uint8_t page_order = 0;
#endif

    if (page_order > PAGE_MAX_ORDER)
        page_order = PAGE_MAX_ORDER;
    while (1)
    {
        page = alloc_page_block(page_order);
        if (page)
        {
            slob_pages_count += PAGES_COUNT(page);
            break;
        }
        if (!page_order)
            break;
        page_order--;
    }

    if (!page)
    {
        WARN("alloc_cache_page failed!");
        return NULL;
    }

    uint16_t free_count = 1;
    free_offs_t size = 1 << order;
    free_offs_t page_size = 1 << (PAGE_SHIFT + page_order);
    void *ptr = page_to_ptr(page);
    free_offs_t *p_offs = (free_offs_t *)ptr;

    for (free_offs_t offs = size; offs < page_size; offs += size)
    {
        *p_offs = offs;
		p_offs += size / sizeof(*p_offs);
        free_count++;
    }
    *p_offs = FREE_OFFS_NULL;

    page->type = PT_SLOB;
    page->cache_idx = order - OBJ_MIN_ORDER;
    page->free_offs = 0;
    page->free_count = free_count;

    return page;
}

static void init_kmem_cache(kmem_cache_t *kcache, uint8_t order)
{
    kcache->alloc = NULL;
    kcache->full = NULL;
#ifdef SLOB_RESERVED_PAGE
    kcache->empty = NULL;
#endif
    spinlock_init(&kcache->lock);
    kcache->order = (uint8_t)order;
}

static void *kmem_cache_alloc(kmem_cache_t *kcache)
{
    lock_acq(&kcache->lock);

    page_t *page = kcache->alloc;

#ifdef SLOB_RESERVED_PAGE
    if (!page)
    {
        kcache->alloc = page = kcache->empty;
        kcache->empty = NULL;
    }
#endif
    if (!page || FREE_OFFS_NULL == page->free_offs) // NULL or full
    {
        page = alloc_cache_page(kcache->order);
        if (!page)
        {
            return NULL;
        }
        page_push(&kcache->alloc, page);
    }

    void *page_data = page_to_ptr(page);
    void *ptr = page_data + page->free_offs;

    page->free_offs = *(free_offs_t *)ptr;
    page->free_count--;

    if (FREE_OFFS_NULL == page->free_offs)  // full
    {
        page_pop(&kcache->alloc);
        page_push(&kcache->full, page);     // move to full pages
    }
    else
    {
        if (page->free_offs >= (1 << (PAGE_SHIFT + page->order)))
        {
            PANIC("kmem_cache_alloc(), page:%p %x(order=%x), page->free_offs: %x >= %x",
                page, page_to_pn(page), page->order, page->free_offs, 1 << (PAGE_SHIFT + page->order));
        }
    }

    lock_rel(&kcache->lock);

    return ptr;
}

static void kmem_cache_free(kmem_cache_t *kcache, page_t *page, void* ptr)
{
    lock_acq(&kcache->lock);

    free_offs_t *p_offs = (free_offs_t *)((uintptr_t)ptr & ~((1 << kcache->order) - 1));

    *p_offs = page->free_offs;
    page->free_offs = (free_offs_t)((uintptr_t)p_offs - (uintptr_t)page_to_ptr(page));
    if (page->free_offs >= (1 << (PAGE_SHIFT + page->order)))
    {
        PANIC("kmem_cache_free(%p), page:%p %x(order=%x), page->free_offs: %x >= %x", ptr,
            page, page_to_pn(page), page->order, page->free_offs, 1 << (PAGE_SHIFT + page->order));
    }
    page->free_count++;

    if (page_remove(&kcache->full, page))
    {
        page_push_front(&kcache->alloc, page);   // move to head of active pages
    }
    else
    {
        uint32_t max_count = 1 << (page->order + PAGE_SHIFT - kcache->order);

        if (max_count == (uint32_t)page->free_count)  // empty page
        {
            if (page != kcache->alloc)
            {
                page_remove(&kcache->alloc, page);
                slob_pages_count -= PAGES_COUNT(page);
#ifdef SLOB_RESERVED_PAGE
                if (kcache->empty)
                    free_page_block(kcache->empty);
                kcache->empty = page;
#else
                free_page_block(page);
#endif
            }
        }
    }

    lock_rel(&kcache->lock);
}

void kmem_cache_info(kmem_cache_t *kcache)
{
    if (!kcache)
    {
        kcache = &kmem_cache_this;
    }

    page_t *page = kcache->alloc;

    if (!page)
        return;

    KINFO("page 0x%x: %p PO: %u, free count: %u, free offs: 0x%x\n",
        page_to_pn(page), page_to_ptr(page), page->order, page->free_count, page->free_offs);

    if (!page->free_count)
        return;

#ifdef USING_DEBUG
    void *page_data = page_to_ptr(page);
    free_offs_t step = 1 << (kcache->order - 2);
    free_offs_t *p_offs = (free_offs_t *)(page_data + page->free_offs);

    for (int i = 0; *p_offs != FREE_OFFS_NULL; i++, p_offs += step)
    {
        KDBG(" %x", *p_offs);
        if (15 == (i & 15))
            KDBG("\n");
        if (!*p_offs && p_offs != page_data)
            PANIC("free offset failed!\n");
    }
    if (15 != (page->free_count & 15))
        KDBG("\n");
#endif
}

void kmem_cache_all_info(void)
{
    KINFO("----------------------------------------------------------------\n");
    for (int i = 0; i < OBJ_ORDERS; i++)
    {
        kmem_cache_t *kcache = kmem_cache[i];

        if (!kcache)
            continue;
        KINFO("kcache%d KO: %u, ", i, kcache->order);
        kmem_cache_info(kcache);
    }
    KINFO("kmem_cache_this KO: %u, ", kmem_cache_this.order);
    kmem_cache_info(NULL);
    KINFO("----------------------------------------------------------------\n");
}

void kmem_cache_init(void)
{
    memset(kmem_cache, 0, sizeof(kmem_cache));
    spinlock_init(&kmem_cache_lock);
    slob_pages_count = 0;

    uint8_t order = get_order(sizeof(kmem_cache_this));
    init_kmem_cache(&kmem_cache_this, order);
    kmem_cache_info(NULL);
}

#ifdef SLOB_DEBUG
static int slog_dbgcnt = 0;
#endif
void *kmalloc(size_t size, uint32_t flags)
{
    //	void* ptr = NULL;
    uint8_t order = get_order(size);

    if (order > OBJ_MAX_ORDER)
    {
        size_t page_count = DIV_ROUNDUP(size, PAGE_SIZE);

        paddr_t paddr = alloc_pages(page_count, ((flags & (~MEM_TYPE_MASK)) | KMEM) );

        if (NULL_PADDR == paddr)
        {
            return NULL;
        }

        return paddr2ptr(paddr);
    }
    else
    {
        if (order <= OBJ_MIN_ORDER)
        {
            order = OBJ_MIN_ORDER;
        }

        if (!kmem_cache_this.order) // before kmem_cache_init
        {
            extern paddr_t early_calloc(size_t size, unsigned align);
            paddr_t paddr = early_calloc(1 << order, 1 << order);

            if (NULL_PADDR == paddr)
            {
                return NULL;
            }

            return paddr2ptr(paddr);
        }

        uint8_t idx = order - OBJ_MIN_ORDER;

        lock_acq(&kmem_cache_lock);
        if (!kmem_cache[idx])
        {
            kmem_cache[idx] = (kmem_cache_t *)kmem_cache_alloc(&kmem_cache_this);
            init_kmem_cache(kmem_cache[idx], order);
        }
        lock_rel(&kmem_cache_lock);

        if (kmem_cache[idx])
        {
            void *ptr = kmem_cache_alloc(kmem_cache[idx]);
#ifdef SLOB_DEBUG
            if (188 ==slog_dbgcnt)
                slog_dbgcnt++;
            KDBG("kmallocz: %p + 0x%x, %u\n", ptr, size, slog_dbgcnt++);
            kmem_cache_info(kmem_cache[idx]);
#endif
            if (ptr && (flags & MEM_FLAGS_ZERO))
                memset(ptr, 0, size);
            return ptr;
        }
    }

    return NULL;
}

void kfree(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    paddr_t paddr = ptr2paddr(ptr);

    if (NULL_PADDR == paddr)
    {
        WARN("ptr: %p not a kernel pointer", ptr);
        return;
    }
    page_t *page = ptr_to_page(ptr);
    page_t *head = page;

    for (int i = 0; i < (1 << PAGE_MAX_ORDER); i++)
    {
        if (head->type != PT_ELEM)
            break;
        head--;
    }

    if (PT_SLOB == head->type)
    {
#ifdef SLOB_DEBUG
        kmem_cache_free(kmem_cache[head->cache_idx], head, ptr);
        KDBG("kfree: %p, %u\n", ptr, slog_dbgcnt++);
        kmem_cache_info(kmem_cache[head->cache_idx]);
#else
        kmem_cache_free(kmem_cache[head->cache_idx], head, ptr);
#endif
    }
    else
    {
        free_pages(paddr);
    }
}

size_t mem_size(const void *ptr)
{
    if (!ptr)
    {
        return 0;
    }

    page_t *page = ptr_to_page(ptr);

    for (int i = 0; i < (1 << PAGE_MAX_ORDER); i++)
    {
        if (page->type != PT_ELEM)
            break;
        page--;
    }

    if (PT_SLOB == page->type)
    {
        return (size_t)1LL << kmem_cache[page->cache_idx]->order;
    }

    return (size_t)count_page_block(page) << PAGE_SHIFT;
}

void *realloc(void *ptr, size_t new_size) 
{
    size_t size = mem_size(ptr);

    if (!size){
        return NULL;
    }

	if (new_size < size){
		return ptr;
	}

    void *new_ptr = kmalloc(new_size, KMEM);

    if (!new_ptr)
    {
        return NULL;
    }

    memcpy(new_ptr, ptr, size);
    kfree(ptr);

    return new_ptr;
}
