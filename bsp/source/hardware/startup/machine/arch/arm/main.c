#include <types.h>
#include <libc/include/string.h>
#include "lib/startup.h"


static void print_boot_info(arch_boot_info_t* p_bootinfo)
{
    printf("\nShow Memmap:\n\n");

    for(int i = 0; i < p_bootinfo->memmap_data.entry_counts; i++)
    {
        printf("Found a memory area, from %p to %p, size = %p, attr = %x\n", 
                (size_t)p_bootinfo->memmap_data.entry[i].base,
                (size_t)p_bootinfo->memmap_data.entry[i].base + (size_t)p_bootinfo->memmap_data.entry[i].length,
                (size_t)p_bootinfo->memmap_data.entry[i].length, (size_t)p_bootinfo->memmap_data.entry[i].type);
    }

    printf("\nLoad start at address: %p\n\n", (size_t)p_bootinfo->ker_hdr.prog_hdr[0].lma);
    printf("\nShow Kernel:\n\n");
    printf("Kernel entry = %p\n", (size_t)p_bootinfo->ker_hdr.entry);
    // for (uint8_t j = 0; j < p_bootinfo->ker_hdr.ph_nmbers; j++)
    // {
    //     printf(" sect%d: lma = %p, size = %x\n", j,
    //         (size_t)p_bootinfo->ker_hdr.prog_hdr[j].lma,
    //         p_bootinfo->ker_hdr.prog_hdr[j].file_size ?
    //         (size_t)p_bootinfo->ker_hdr.prog_hdr[j].file_size :
    //         (size_t)p_bootinfo->ker_hdr.prog_hdr[j].mem_size);
    // }

    printf("\nShow Module:\n\n");

    for(int i = 0; i < p_bootinfo->module_number; i++)
    {
        printf("Found a module,name = %s, entry = %p, sections = %d \n", 
                p_bootinfo->mods_hdr[i].name,
                (size_t)p_bootinfo->mods_hdr[i].entry,
                (size_t)p_bootinfo->mods_hdr[i].ph_nmbers);
        // for (uint8_t j = 0; j < p_bootinfo->mods_hdr[i].ph_nmbers; j++)
        // {
        //     printf(" sect%d: lma = %p, size = %x\n", j,
        //         (size_t)p_bootinfo->mods_hdr[i].prog_hdr[j].lma,
        //         p_bootinfo->mods_hdr[i].prog_hdr[j].file_size ?
        //         (size_t)p_bootinfo->mods_hdr[i].prog_hdr[j].file_size :
        //         (size_t)p_bootinfo->mods_hdr[i].prog_hdr[j].mem_size);
        // }
    }

    printf("\nLoad end at address: %p\n\n", (size_t)p_bootinfo->load_end_addr);
}

static void copy_arch_boot_info(syspage_t* p_syspage)
{
    arch_boot_info_t* src_bootinfo = p_syspage->loadinfo;
    char* free_ptr = (char*)p_syspage + p_syspage->size;
    arch_boot_info_t* dst_bootinfo = (arch_boot_info_t*)free_ptr;
    size_t member_size = sizeof(arch_boot_info_t);

    memcpy(dst_bootinfo, src_bootinfo, member_size);
    dst_bootinfo = (arch_boot_info_t*)free_ptr;
    free_ptr += ALIGN_UP(member_size, sizeof(size_t));

    p_syspage->size = free_ptr - (char*)p_syspage;

    p_syspage->loadinfo = dst_bootinfo;
    printf("\nboot info from %p to %p\n\n", src_bootinfo, dst_bootinfo);
}

void main(syspage_t* p_syspage)
{
    copy_arch_boot_info(p_syspage);
    print_boot_info(p_syspage->loadinfo);

    construct_syspage(p_syspage);
}
