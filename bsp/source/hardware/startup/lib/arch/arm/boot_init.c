#include <libc/include/stdio.h>
#include <boot/include/bootinfo.h>
#include <platform/arch/arm/include/intr/gic.h>
#include <platform/arch/arm/include/timer/gpt.h>
#include "debug_dev.h"

#ifndef HZ
 #define HZ 1000
#endif

extern const char __image_start_addr[];
size_t arch_memory_detect(arch_boot_info_t* p_bootinfo, struct meminfo_entry* p_meminfo)
{
    memmap_entry_t* mem_entry_ptr = p_bootinfo->memmap_data.entry;
    uint8_t mementry_cnt= p_bootinfo->memmap_data.entry_counts;
    uint8_t usable_area_cnt = 0 ;
    uint64_t usable_mem_area = 0;

    if (p_bootinfo->load_start_addr == NULL_PADDR)
        p_bootinfo->load_start_addr = (paddr_t)__image_start_addr;
    for(int i = 0; i < mementry_cnt; i++, mem_entry_ptr++)
    {
        if(mem_entry_ptr->type != MEMMAP_USABLE)
        {
            continue;
        }

        p_meminfo->mem_p_regs.list[usable_area_cnt].start = mem_entry_ptr->base;
        p_meminfo->mem_p_regs.list[usable_area_cnt].end = mem_entry_ptr->base + mem_entry_ptr->length;
        printf("\narch_memory_detect(%d) %p ~ %p\n\n", i,
            (size_t)p_meminfo->mem_p_regs.list[usable_area_cnt].start,
            (size_t)p_meminfo->mem_p_regs.list[usable_area_cnt].end);

        usable_mem_area += mem_entry_ptr->length;

        usable_area_cnt ++;
    }

    p_meminfo->mem_p_regs.count = usable_area_cnt;

    printf("\nTotoally %d count / %d MiB memory are marked as usable!\n\n", usable_area_cnt, (size_t)(usable_mem_area >> 20));

    return sizeof(syspage_t) + bsp_device_size(DBG_DEV_INTF_NUMS) + sizeof(arch_boot_info_t);
}

paddr_t arch_get_kernel_entry(arch_boot_info_t* p_bootinfo)
{
    return p_bootinfo->ker_hdr.entry;
}



void system_init(syspage_t* p_syspage)
{
extern void init_aips(void);
extern void arch_cpu_init(void);
extern void init_clocks(void);
extern void init_pinmux(void);
extern void init_serial_mx6q(void);
extern void init_intrinfo(void);
extern void gic_init(void);
extern void gpt_init(GPT_Type *base, enum gpt_comp_channel chan, struct systime_entry* p_systime);
extern void gic_enable_irq(uint32_t nr);
extern void gpt_run(GPT_Type *base, int on);
extern void gpt_enable_interrupt(GPT_Type *base, enum gpt_interrupt_bit bit, int on);
// extern void ser_putstr(const char *str);
    //////////////////////////////////////////

    init_aips();
    arch_cpu_init();
    init_clocks();
    init_pinmux();

    /* Init serial interface */
	// init_serial_mx6q();  // init in uart

    //	system_init_irq_table();

    init_intrinfo();

    gic_init();

    if (!p_syspage->systime.timer_freq)
    {
        p_syspage->systime.timer_freq = HZ;
    }
    gpt_init(GPT1, OUT_COMP1, &p_syspage->systime);

    //	request_irq(GPT1_IRQn, (irq_handler_t)GPT1_COMP1_handle_irq, NULL);

	//gic_enable_irq(UART1_IRQn);
	gic_enable_irq(p_syspage->devices[0]->irq_num);

    gic_enable_irq(GPT1_IRQn);
	gpt_enable_interrupt(GPT1, IR_OF1IE, 1);
	gpt_run(GPT1, 1);

	printf("\ngpt1 start: timer_freq=%u, cycles_per_sec=%u\n", p_syspage->systime.timer_freq, p_syspage->systime.cycles_per_sec);

    //	while(1);
}
