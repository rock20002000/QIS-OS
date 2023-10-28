#include <types.h>
#include <libc/include/string.h>
#include <boot/include/bootinfo.h>
#include <utils/include/print_macros.h>

#include "board_cfg.h"
#include "../extra-boot/ex_boot.h"

extern arch_boot_info_t g_boot_info;

int is_valid_load_addr(uint64_t load_addr)
{
    int ret = 0;

    if( (load_addr >= RAM_ADDR_START) && (load_addr < RAM_ADDR_END))
    {
        ret = 1;
    }

    return ret;
}

void prepare_boot_info(paddr_t load_start_addr, paddr_t load_end_addr)
{
    uint8_t seg = 0;

    ASSERT(is_valid_load_addr(load_start_addr) && is_valid_load_addr(load_end_addr));
    g_boot_info.load_start_addr = load_start_addr;
    g_boot_info.load_end_addr   = load_end_addr;

    /*Fill the memory map settings of this board!*/
    g_boot_info.memmap_data.entry[seg].base   = IO_ADDR_START;
    g_boot_info.memmap_data.entry[seg].length = IO_ADDR_END - IO_ADDR_START + 1;
    g_boot_info.memmap_data.entry[seg].type   = MEMMAP_IO_ZONE;
    seg++;

    if (load_start_addr > RAM_ADDR_START)
    {
        g_boot_info.memmap_data.entry[seg].base   = RAM_ADDR_START;
        g_boot_info.memmap_data.entry[seg].length = load_start_addr - RAM_ADDR_START;
        g_boot_info.memmap_data.entry[seg].type   = MEMMAP_USABLE;   
        seg++;
    }
    g_boot_info.memmap_data.entry[seg].base   = load_start_addr;
    g_boot_info.memmap_data.entry[seg].length = load_end_addr - load_start_addr;
    g_boot_info.memmap_data.entry[seg].type   = MEMMAP_KERNEL_AND_MODULES;   
    seg++;

    if (load_start_addr < RAM_ADDR_END)
    {
        g_boot_info.memmap_data.entry[seg].base   = load_end_addr;
        g_boot_info.memmap_data.entry[seg].length = RAM_ADDR_END - load_end_addr + 1;
        g_boot_info.memmap_data.entry[seg].type   = MEMMAP_USABLE;
        seg++;
    }

    g_boot_info.memmap_data.entry_counts    = seg;

    /* Set the kernel info data */
    memcpy(&g_boot_info.ker_hdr, &g_kernel_hdr, sizeof(kernel_hdr_t));

    /* Prepare app information */
    g_boot_info.module_number = g_image_hdr.module_number;
    memcpy(g_boot_info.mods_hdr, g_app_hdrs, g_boot_info.module_number*sizeof(application_hdr_t));
}

paddr_t get_exboot_end_addr(void)
{
    return (paddr_t)EXBOOT_ADDR_END;
}