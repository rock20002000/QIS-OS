    #include <types.h>
#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <boot/include/bootinfo.h>

#include "../extra-boot/ex_boot.h"


extern void prepare_boot_info(paddr_t load_start_addr, paddr_t load_end_addr);

arch_boot_info_t g_boot_info;


static inline uint32_t arm_sctlr_get()
{
	uint32_t	val;
	__asm__ __volatile__("mrc	p15, 0, %0, c1, c0, 0" : "=r" (val));
	return val;
}
static inline uint32_t arm_cpsr(void)
{
	uint32_t sr;
	__asm__ __volatile__("mrs	%0, cpsr" : "=r" (sr));
	return sr;
}


#include "debug_dev.h"
#include "../startup/lib/startup.h"
void _boot_main(void)
{
	debug_init(NULL);
#ifdef WIKA_BOARD
	uint32_t gpio_base = WATCHDOG_DEV_BASE;	//	0x0209C000;
	out32(gpio_base + 0x04, in32(gpio_base + 0x04) | (0x1 << 29));	//	MX6X_GPIO_GDIR 0x04
	out32(gpio_base + 0x04, in32(gpio_base + 0x04) & (~(0x1 << 29)));
#endif
	printf(" \nWelcome to extraboot! \nExtracting image now, Please wait...\n");

    uint64_t kef_start_addr = ALIGN_UP(get_exboot_end_addr(), CONFIG_SECTOR_SIZE);

    printf("Start to parse image header from %x, SCTLR: %x, CPSR: %x\n", (uint32_t)kef_start_addr, arm_sctlr_get(), arm_cpsr());

    int64_t ret = kirk_load_image(0, &kef_start_addr);

    if(ret > 0)
    {
        prepare_boot_info((paddr_t)kef_start_addr, (paddr_t)ret);
        
        printf("\n=== From boot,Ready to start bsp ===\n");

        run_next(&g_boot_info, g_bsp_hdr.entry);
    }
    else
    {

    }

    while(1);
    
}
