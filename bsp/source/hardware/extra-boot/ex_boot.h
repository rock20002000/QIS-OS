#ifndef __EXTRA_BOOT_H__
#define __EXTRA_BOOT_H__

#include <boot/include/bootinfo.h>

extern void run_next(arch_boot_info_t* p_bootinfo, paddr_t entry);

extern int is_valid_load_addr(uint64_t load_addr);

extern paddr_t get_exboot_end_addr(void);

/* Defined in $(arch)/$(machine)/driver_block.c */
extern int machine_block_read(int drive, void *buffer, uint64_t loc, size_t count);

extern int block_read(int drive, void *buffer, uint64_t loc, size_t count);


/* Defined in read_kef.c */
extern image_hdr_t     g_image_hdr;
extern kernel_hdr_t    g_kernel_hdr;
extern bsp_hdr_t       g_bsp_hdr;
extern application_hdr_t g_app_hdrs[CONFIG_MAX_MODULES];

extern int64_t kirk_load_image(uint8_t block_device, uint64_t* p_image_loc);

#endif