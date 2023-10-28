#ifndef _STARTUP_H_INCLUDED
#define _STARTUP_H_INCLUDED

#include <types.h>
#include <boot/include/bootinfo.h>

#include <utils/include/print_macros.h>


#define crash(fmt, ...)     printf(fmt, ##__VA_ARGS__);crash_done();

extern void			(*mdriver_check)(void);

//weak_defines.c
void board_init(syspage_t* p_syspage);
void print_char_default(int);
void crash_done(void);


//init_raminfo.c
void init_raminfo(arch_boot_info_t* p_bootinfo, struct meminfo_entry* p_meminfo);
paddr_t alloc_ram(struct meminfo_entry* p_meminfo, paddr_t addr, paddr_t size, size_t align);
paddr_t calloc_ram(struct meminfo_entry* p_meminfo, size_t size, size_t align);

// syspage.c
extern void construct_syspage(syspage_t* p_syspage);

// _main.c
// extern syspage_t* bsp_syspage_ptr;
/**/
extern paddr_t easy_calloc(size_t size);

// //Machine/arch-dir/main.c
extern void main(syspage_t* p_syspage);

//arch/$(ARCH)/start_next.S
extern void start_next(void* p_sysinfo, uint8_t cpu_index, vaddr_t kernel_entry, vaddr_t kernel_stack);

//arch/$(ARCH)/processor64.c
void arch_init_pgdir(syspage_t* p_syspage);
void arch_load_pgdir(syspage_t* p_syspage, unsigned cpu_idx);
// void arch_map_kernel_sections(uintptr_t vaddr, paddr_t paddr, size_t size);
void arch_map_kernel(arch_boot_info_t* p_bootinfo);
vaddr_t arch_paddr2vaddr(paddr_t paddr);

//arch/$(ARCH)/boot_init.c
size_t arch_memory_detect(arch_boot_info_t* p_bootinfo, struct meminfo_entry* p_meminfo);
paddr_t arch_get_kernel_entry(arch_boot_info_t* p_bootinfo);

//arch/$(ARCH)/boot_init.c
void system_init(syspage_t* p_syspage);

void kprintf(const char *fmt, ...);

// //Machine/arch-dir/board_spec.c
void debug_init(bsp_device_t* p_bsp_dev);
void debug_putc(bsp_device_t* p_bsp_dev, int ch);

#define _BITFIELD64B(__start_bit,__value) ((_Uint64t)(__value) << (63-(__start_bit)))
#define _BITFIELD32B(__start_bit,__value) ((__value) << (31-(__start_bit)))
#define _BITFIELD16B(__start_bit,__value) ((__value) << (15-(__start_bit)))
#define _BITFIELD8B(__start_bit,__value) ((__value) << ( 7-(__start_bit)))

#define _BITFIELD64L(__start_bit,__value) ((_Uint64t)(__value) << (__start_bit))
#define _BITFIELD32L(__start_bit,__value) ((__value) << (__start_bit))
#define _BITFIELD16L(__start_bit,__value) ((__value) << (__start_bit))
#define _BITFIELD8L(__start_bit,__value) ((__value) << (__start_bit))

#define _ONEBIT64B(__start_bit)    _BITFIELD64B(__start_bit,1)
#define _ONEBIT32B(__start_bit)    _BITFIELD32B(__start_bit,1)
#define _ONEBIT16B(__start_bit)    _BITFIELD16B(__start_bit,1)
#define _ONEBIT8B(__start_bit)    _BITFIELD8B(__start_bit,1)

#define _ONEBIT64L(__start_bit)    _BITFIELD64L(__start_bit,1)
#define _ONEBIT32L(__start_bit)    _BITFIELD32L(__start_bit,1)
#define _ONEBIT16L(__start_bit)    _BITFIELD16L(__start_bit,1)
#define _ONEBIT8L(__start_bit)    _BITFIELD8L(__start_bit,1)

#endif
