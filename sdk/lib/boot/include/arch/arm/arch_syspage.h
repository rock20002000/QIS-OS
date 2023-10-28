#ifndef __ARCH_SYSPAGE_H__
#define __ARCH_SYSPAGE_H__

#include <types.h>
#include <boot/include/kirk_image_def.h>
#include <mmu_arm32.h>

#define KERNEL_VADDRESS_START        0xC1000000
#define KERNEL_VADDRESS_END          0xF8000000


typedef struct memmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
} memmap_entry_t;

#define MAX_MEMMAP_ENTRIES      8

typedef struct memmap_data {
    memmap_entry_t entry[MAX_MEMMAP_ENTRIES];
    uint8_t entry_counts;
    uint8_t reserved[7];
} memmap_data_t;


typedef struct arch_boot_info{
    arm_mmu_attr_t      mmu_attr;
    memmap_data_t       memmap_data;
    kernel_hdr_t        ker_hdr;
    application_hdr_t   mods_hdr[CONFIG_MAX_MODULES];
    paddr_t             load_start_addr;
    paddr_t             load_end_addr;
    size_t              ram_map_size;
    vaddr_t             io_start_vaddr;
    size_t              io_map_size;

    uint8_t module_number;
    uint8_t reserved[7];
} arch_boot_info_t;

#endif
