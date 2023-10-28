#include <libc/include/string.h>
#include "lib/startup.h"
#include "board.h"


#define PADDR_1TO1_PTR(p_meminfo, paddr)    ((void *)((paddr_t)(paddr) + (p_meminfo)->kvaddr_offs))

static int add_mem(struct meminfo_entry* p_meminfo, paddr_t start, paddr_t size) {
    paddr64_t	 		 e, s;
    struct ram_entry	*r;
    struct ram_entry	*ram = p_meminfo->ram_list;
    int ram_index = 0;
    unsigned  ram_list_size = p_meminfo->ram_list_size;
    unsigned  last_ram_list_size;

    if(size > 0) {
        
        if(0 == ram_list_size)
        {
            ram[0].addr = start;
            ram[0].size = size;
            ram_list_size = 1;
            p_meminfo->ram_list_size = ram_list_size;
            return 0;
        }
        
        for(ram_index = 0, r = ram; ram_index < ram_list_size; r++) {
            if(r->addr > start) break;
            ram_index++;
        }
        
        
        if(ram_list_size + 1 > (MAX_RAM_LIST_SIZE / sizeof(*r)) ) {
            DBG("RAM entry array overflow, and should crash here.");
        }
        
        last_ram_list_size = ram_list_size - ram_index;
        memmove(&r[1], r, last_ram_list_size * sizeof(*r) );
        ram_list_size += 1;
        r->addr = start;
        r->size = size;

        for(ram_index = 1, r = ram + 1; ram_index < ram_list_size; r++)
        {
            e = r[-1].addr + r[-1].size;
            s = r->addr;
            if(s <= e) {
                if((s + r->size) > e)  e = s + r->size;	
                last_ram_list_size = ram_list_size - ram_index  - 1;
                memmove(r, &r[1], last_ram_list_size * sizeof(*r) );
                ram_list_size -= 1;
                --r;
                r->size = e - r->addr;
            }
            ram_index++;
        }
    }
    
    p_meminfo->ram_list_size = ram_list_size;
    
    return 0;
}


void add_avoid_ram(struct meminfo_entry* p_meminfo, paddr64_t start, size_t size) {
    
    struct avoid_entry		*avoid;
    struct avoid_entry		*avoid_list = p_meminfo->avoid_list;
    unsigned				avoid_list_size = p_meminfo->avoid_list_size;
    int index;
    paddr64_t   end = start + size;
    
    for(index = 0; index < avoid_list_size; index++)
    {
        if(start > avoid_list[index].start && end < avoid_list[index].end)
            break;
        
        if( end > avoid_list[index].start &&  end < avoid_list[index].end )
        {
            if( avoid_list[index].start >  start)
                avoid_list[index].start = start;
            break;
        }
        if(start > avoid_list[index].start && start < avoid_list[index].end)
        {
            if( avoid_list[index].end >  end)
                avoid_list[index].end = end;
            
            break;
        }
    }
    if(index == avoid_list_size)
    {
        avoid = avoid_list + avoid_list_size;
        avoid->start = start;
        avoid->end = start + size;
        p_meminfo->avoid_list_size++;
    }
}

void init_raminfo(arch_boot_info_t* p_bootinfo, struct meminfo_entry* p_meminfo)
{
    uint8_t count, mem_index;

    // physical_memory_init();

    paddr_t start_avoid = ROUND(p_bootinfo->load_end_addr, sizeof(uint64_t));
    p_meminfo->avoid_list = (struct avoid_entry*)PADDR_1TO1_PTR(p_meminfo, start_avoid);
    start_avoid +=  ROUND(MAX_AVOID_LIST_SIZE, sizeof(uint64_t));
#ifdef BOOTUP_ELF_PADDR_BASE
    p_meminfo->avoid_list[0].start  = BOOTUP_ELF_PADDR_BASE;
    p_meminfo->avoid_list[0].end = start_avoid;
    p_meminfo->avoid_list_size = 1;
    DBG("bootup avoid  from start: 0x%p to end: 0x%p\n", (size_t)BOOTUP_ELF_PADDR_BASE, (size_t)p_bootinfo->load_end_addr);
#else
    p_meminfo->avoid_list_size = 0;
#endif
    p_meminfo->ram_list =  (struct ram_entry*)PADDR_1TO1_PTR(p_meminfo, start_avoid);
    p_meminfo->ram_list_size = 0;
    add_avoid_ram(p_meminfo, start_avoid, MAX_RAM_LIST_SIZE);

    KDBG("avoid_list: @0x%p, ram_list: @0x%p\n", p_meminfo->avoid_list, p_meminfo->ram_list);

    p_meminfo->alloc_top_paddr = start_avoid + MAX_RAM_LIST_SIZE;

    count = p_meminfo->mem_p_regs.count;	
    for(mem_index = 0; mem_index < count; mem_index++)
    {
        DBG("RAM==>start: 0x%p, end: 0x%p\n",
            (size_t)p_meminfo->mem_p_regs.list[mem_index].start,
            (size_t)p_meminfo->mem_p_regs.list[mem_index].end);

        paddr_t start;
        paddr_t  size;
        start = p_meminfo->mem_p_regs.list[mem_index].start;
        size = p_meminfo->mem_p_regs.list[mem_index].end - p_meminfo->mem_p_regs.list[mem_index].start;
        add_mem(p_meminfo, start, size);
    }
    for(int index = 0; index < p_meminfo->avoid_list_size; index++)
        DBG("avoid list addr: 0x%p, end: 0x%p\n", p_meminfo->avoid_list[index].start, p_meminfo->avoid_list[index].end);
    for(int index = 0; index < p_meminfo->ram_list_size; index++)
        DBG("ram list addr: 0x%p, size: 0x%p\n", p_meminfo->ram_list[index].addr, p_meminfo->ram_list[index].size); 
}


static int
fix_size(paddr64_t *sp, paddr64_t *ep, paddr_t as, paddr_t ae) {
    paddr64_t		s = *sp;
    paddr64_t		e = *ep;

    if((s >= ae) || (e <= as)) {
        return 1;
    }
    if(s >= as) {
        if(e <= ae) {
            return 0;
        }
        *sp = ae;
    } else if(e < ae) {
        *ep = as;
    } else if((as-s) > (e-ae)) {
        *ep = as;
    } else {
        *sp = ae;
    }
    return 1;
}

paddr_t
find_ram_in_range(struct meminfo_entry* p_meminfo, paddr_t range_start, paddr_t range_end, size_t size, size_t align)
{
    struct ram_entry		*ram = p_meminfo->ram_list;
    struct ram_entry		*r;
    paddr64_t 				s;
    paddr64_t 				e;
    paddr_t					addr;
    int 					ram_index = 0;
    
    unsigned				avoid_list_size = p_meminfo->avoid_list_size;
    struct avoid_entry		*avoid_list = p_meminfo->avoid_list;
    struct avoid_entry		*a;
    unsigned                avoid_index = 0;


    for(r = ram; ram_index < p_meminfo->ram_list_size; r++, ram_index++){
        int avoided=0;
        s = r->addr;
        e = s + r->size;
        
        for(a = avoid_list, avoid_index = 0; avoid_index < avoid_list_size; avoid_index++, ++a){
            if(!fix_size(&s, &e, a->start, a->end)) {
                avoided=1;
                break;
            }
        }
        
        if (avoided) {
            continue;
        }
                
        addr = ROUND(s, align);
        if (range_start) {
            if (addr < (paddr_t) range_start) {
                addr = range_start;
            }
        }
        if (range_end) {
            if (e > (paddr_t) range_end) {
                e = range_end;
            }
        }

        if(addr >= e) {
            continue;
        }
        if(size <= (e-addr)) {
            return addr;
        }
    }
    
    return NULL_PADDR;
}

paddr_t
find_ram(struct meminfo_entry* p_meminfo, size_t size, size_t align) {
    paddr_t result;
    result = find_ram_in_range(p_meminfo, 0, (paddr_t)~(paddr_t)0, size, align);
    if (result == NULL_PADDR) {
        return NULL_PADDR;
    }
    return (paddr_t) result;
}

void *
ws_alloc(struct meminfo_entry* p_meminfo, size_t size) {
    
    paddr_t	paddr;
    size = ROUND(size, sizeof(uint64_t));
    paddr = find_ram(p_meminfo, size, sizeof(uint64_t));
    if(paddr == NULL_PADDR) {
        return(NULL);
    }
    add_avoid_ram(p_meminfo, paddr, size);
    return(PADDR_1TO1_PTR(p_meminfo, paddr));	
}

paddr_t
alloc_ram(struct meminfo_entry* p_meminfo, paddr_t addr, paddr_t size, size_t align) {
    
    struct ram_entry	*ram = p_meminfo->ram_list;
    struct ram_entry	*r;
    paddr64_t 			s, e, end;
    if(addr == NULL_PADDR) {
        addr = find_ram(p_meminfo, size, align);
        if(addr == NULL_PADDR) {
            return NULL_PADDR;
        }
    }
    int 		ram_list_index;
    unsigned	last_ram_list_size;
    end 	= 	addr + size;
    for(ram_list_index = 0, r = ram; ram_list_index < p_meminfo->ram_list_size; ram_list_index++, r++)
    {
        s = r->addr;
        e = s + r->size;
        if((addr <= s && end > s)
        || (addr >= s && addr < e)
        || (end  >= s && end  < e)) {
            if(end  > e) e=end;
            if(addr < s) s=addr;
            if(addr == s && end == e) {
                last_ram_list_size = p_meminfo->ram_list_size - ram_list_index  - 1;
                memmove(r, &r[1], last_ram_list_size * sizeof(*r) );
                p_meminfo->ram_list_size -= 1;
                --r;
            } else if(addr == s) {
                r->size = e - end;
                r->addr = end;
            } else {
                r->size = addr - s;
                if(e != end) {
                    add_mem(p_meminfo, end, e - end);
                }
            }
        }
    }

    return addr;
}

paddr_t
calloc_ram(struct meminfo_entry* p_meminfo, size_t size, size_t align) {
    paddr_t a;

    a = alloc_ram(p_meminfo, NULL_PADDR, size, align);
    if((a != NULL_PADDR)) {
        if (a + size > p_meminfo->alloc_top_paddr)
            p_meminfo->alloc_top_paddr = a + size;
        memset(PADDR_1TO1_PTR(p_meminfo, a), 0, size);
    }
    return a;
}
