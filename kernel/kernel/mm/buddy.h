#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <stdint.h>
#include "arch_mm.h"
#include "debug.h"

typedef uint16_t    poff_t;

enum page_type
{
    PT_RESERVE, // reserved page
    PT_FREE,    // free page
    PT_ELEM,    // element page of block
    PT_HEAD,    // head page of blocks
    PT_SLOB     // head page of slob blocks
};

typedef uint32_t    free_offs_t;

typedef struct page 
{
    struct page *next;
    uint16_t    ref_cnt;
    uint8_t     type;
    uint8_t     order;
#if 0
    struct {
        uint32_t    free_offs   : 20;
        uint32_t    free_count  : 9;
        uint32_t    cache_idx   : 3;
    };  // for slab
#else
    // For slob
    free_offs_t free_offs;
    uint16_t    free_count;
    uint16_t    cache_idx;
#endif
} page_t;

#define PAGES_COUNT(p)          (1L << (p)->order)
#define ALLOC_MAX_PAGES         (1L << (PAGE_MAX_ORDER - 1))

extern struct page *g_pages;

#define pn_to_page(pn)          (g_pages + (size_t)(pn))
#define page_to_pn(pg)          ((pfn_t)((page_t *)(pg) - g_pages))
#define paddr_to_pn             addr_to_pn
#define pn_to_paddr             pn_to_addr
#define paddr_to_page(paddr)    pn_to_page(paddr_to_pn(paddr))
#define page_to_paddr(pg)       pn_to_paddr(page_to_pn(pg))

static inline void page_push(page_t **p_head, page_t *page)
{
    page->next = *p_head;
    *p_head = page;
}

static inline void page_push_front(page_t **p_head, page_t *page)
{
    page_t *prev = *p_head;

    if (!prev)
    {
        *p_head = page;
    }
    else
    {
        while (prev->next)
        {
            prev = prev->next;
        }
        prev->next = page;
    }
}

static inline page_t *page_pop(page_t **p_head)
{
    page_t *page = *p_head;

    if (page)
    {
        *p_head = page->next;
        page->next = NULL;
    }
    return page;
}

static inline bool page_remove(page_t **p_head, page_t *page)
{
    page_t *prev = *p_head;

    if (prev == page)
    {
        *p_head = prev->next;
        page->next = NULL;
        return true;
    }

    while (prev && prev->next)
    {
        if (page == prev->next)
        {
            prev->next = page->next;
            page->next = NULL;
            return true;
        }
        prev = prev->next;
    }

    return false;
}

static inline uint32_t pages_count(page_t *page)
{
    if (!page)
    {
        return 0;
    }
    
    uint32_t count = 1;

    while (page->next)
    {
        count++;
        page  = page->next;
    }
    return count;
}

extern page_t *alloc_page_block(uint8_t order);
extern uint32_t free_page_block(page_t *page);
extern uint32_t count_page_block(page_t *page);
extern void page_info(const page_t *page);
extern uint32_t paddr_pages(paddr_t paddr);

#endif
