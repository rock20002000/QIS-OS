
#include "arch.h"



//Create a bootinfo variate to save boot infomation in kernel space in case low-address memory has been over-written
// struct bootinfo g_bootinfo;

void store_bootinfo(void) 
{
    
    // offset_t off = 0;
    // for (int i = 0; i < NUM_BOOTINFO_MEMMAP_MAX; i++) {
    //     if (off >= boot_info->mmap_len) {
    //         break;
    //     }

    //     struct bootinfo_memmap_entry *m = &g_bootinfo.memmap[i];
    //     struct multiboot_mmap_entry *e = paddr2ptr(boot_info->mmap_addr + off);
    //     m->base = e->base;
    //     m->len = e->len;
    //     m->type = (e->type == 1) ? BOOTINFO_MEMMAP_TYPE_AVAILABLE
    //                              : BOOTINFO_MEMMAP_TYPE_RESERVED;

    //     if ( (m->base + m->len <= (vaddr_t) __kernel_image_end) 
    //         && (m->type == BOOTINFO_MEMMAP_TYPE_AVAILABLE) )
    //     {
    //         m->type = BOOTINFO_MEMMAP_TYPE_KRESERVED;
    //     }
    //     else
    //     {
    //         //So 0~__kernel_image_end low memory adress is reserved for kernel

    //         if (m->base < (vaddr_t) __kernel_image_end) 
    //         {
    //             m->reserved[0] = 1;

    //             m->len -= (vaddr_t) __kernel_image_end - m->base;

    //             m->base = (vaddr_t) __kernel_image_end;
    //         }
    //     }

    //     off += e->entry_size + sizeof(uint32_t);
    // }

    // g_bootinfo.mods_count = boot_info->mods_count;
    // g_bootinfo.mod_addr = boot_info->mods_addr;
}   