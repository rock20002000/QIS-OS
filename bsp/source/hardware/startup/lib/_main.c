#include <stdint.h>
#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <boot/include/bootinfo.h>

#include "lib/startup.h"

extern char __boot_image_end[];

syspage_t* bsp_syspage_ptr = NULL;

static void hook_dummy(void) {}

void (*mdriver_check)(void) = hook_dummy;

paddr_t easy_calloc(size_t size)
{
    paddr_t ret = 0;
    if(NULL == bsp_syspage_ptr)
    {
        ret =  NULL_PADDR;
    }
    else
    {
        ret = calloc_ram(&bsp_syspage_ptr->meminfo, size, 1);
    }
    return ret;
}

void _main(void* ptr_bootinfo)
{
    uint8_t cpu_idx = 0;//get_cpuid();
    arch_boot_info_t* p_bootinfo = (arch_boot_info_t*)ptr_bootinfo;

    if (!bsp_syspage_ptr)
    {
        struct meminfo_entry _meminfo;
        // Read memory info from p_bootinfo passed from ipl
        size_t syspage_size = arch_memory_detect(p_bootinfo, &_meminfo);

        init_raminfo(p_bootinfo, &_meminfo);
        bsp_syspage_ptr = (syspage_t*)calloc_ram(&_meminfo, syspage_size, __PAGESIZE);
        memcpy(&bsp_syspage_ptr->meminfo, &_meminfo, sizeof(_meminfo));
        // bsp_syspage_ptr->size = (uint16_t)syspage_size;
        bsp_syspage_ptr->size = sizeof(syspage_t);
    }

    if(0 == cpu_idx)
    {   // bootstrap processor?
        bsp_syspage_ptr->loadinfo = p_bootinfo;

        board_init(bsp_syspage_ptr);

        DBG("\n=== Welcome to bsp world! ===\n\n");

        printf("Received boot infomation located at %p, parse it to syspage %p\n", p_bootinfo, bsp_syspage_ptr);

        main(bsp_syspage_ptr);
    }


    ////////////////////////////////////////////////

    system_init(bsp_syspage_ptr);

    ////////////////////////////////////////////////

    /// Enable MMU and jump to kernel
    // Load page-dir prepared for kernel.
    arch_load_pgdir(bsp_syspage_ptr, cpu_idx);

    vaddr_t kernel_entry = arch_get_kernel_entry(bsp_syspage_ptr->loadinfo);
    vaddr_t kernel_stack = arch_paddr2vaddr((paddr_t)__boot_image_end);

    //todo: add smp surpport
    /*Jmp to kernel zone if syspage is ready*/
    DBG("===== Attempt to start kernel! God bless me! =====\n");
    start_next(bsp_syspage_ptr, cpu_idx, kernel_entry, kernel_stack);
    
    while (1)
    {
        /* Will never run to here */
    }
}
