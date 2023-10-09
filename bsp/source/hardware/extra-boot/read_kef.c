#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <boot/include/bootinfo.h>
#include "board_cfg.h"
#include "./ex_boot.h"

image_hdr_t     g_image_hdr;
kernel_hdr_t    g_kernel_hdr;
bsp_hdr_t       g_bsp_hdr;
application_hdr_t g_app_hdrs[CONFIG_MAX_MODULES];



static void load_program_sections(uint8_t block_device, uint64_t image_loc, program_hdr_t* phdrs_ptr, uint8_t ph_nmbers,int64_t entry_offset,uint8_t is_kernel)
{
    for (int i = 0; i < ph_nmbers; i++)
    {
        uint64_t prog_section_start_addr = phdrs_ptr[i].lma + entry_offset;

        if(phdrs_ptr[i].mem_size)
        {
            uint64_t bss_size = 0;

            if(phdrs_ptr[i].mem_size > phdrs_ptr[i].file_size)
            {
                bss_size = phdrs_ptr[i].mem_size - phdrs_ptr[i].file_size;
            }

            block_read(block_device, (void*)(uint32_t)prog_section_start_addr, image_loc + phdrs_ptr[i].offset, phdrs_ptr[i].file_size);

            printf("Loaded program section [index = %d],from %x to %x,size = %x\n", i,
                (paddr_t)prog_section_start_addr,
                (paddr_t)(prog_section_start_addr + phdrs_ptr[i].file_size),
                (size_t)phdrs_ptr[i].file_size);

            if(bss_size && is_kernel)
            {
                memset((void*)(uint32_t)(prog_section_start_addr + phdrs_ptr[i].file_size), 0, bss_size);

                printf("Initialized bsp/kernel bss section from %x to %x\n",
                    (paddr_t)(prog_section_start_addr + phdrs_ptr[i].file_size),
                    (paddr_t)(prog_section_start_addr + phdrs_ptr[i].mem_size));
            }
        }

        /* Update lma to real load memory address */
        phdrs_ptr[i].lma = prog_section_start_addr;
    }
}

int64_t kirk_load_image(uint8_t block_device, uint64_t* p_image_loc)
{
    image_hdr_t* p_imghdr = NULL;
    uint8_t sector_read_buff[CONFIG_SECTOR_SIZE];
    uint64_t image_loc = *p_image_loc;

    /*Load the 512-byte image header to read buffer and try to parse it!*/
    block_read(block_device, (void*)sector_read_buff, image_loc, CONFIG_SECTOR_SIZE);

    p_imghdr = &g_image_hdr;
    memcpy(p_imghdr, sector_read_buff, sizeof(image_hdr_t));
    /* Check the magic to detect if it is a kirk-image */
    if( memcmp(p_imghdr->magic, CONFIG_KIRKIMAGE_MAGIC, sizeof(p_imghdr->magic)) )
    {
        printf("Failed to load a kirk-image from the %dth sector of hdd device: %d \n", (int32_t)(image_loc/CONFIG_SECTOR_SIZE), (int32_t)block_device);
        return -1;
    }

    printf("Try to load kirk image,name = %s\n", p_imghdr->img_name);

    /* Save the image header infomation,-- */
    if(p_imghdr->module_number > CONFIG_MAX_MODULES)
    {
        printf("Read an invalid num_of_mods(%d),Must be less than CONFIG_MAX_MODULES(%d)\n", (int32_t)p_imghdr->module_number, (int32_t)CONFIG_MAX_MODULES);
        return -1;
    }

    uint64_t bsp_load_min_addr = -1ULL;
    uint64_t bsp_load_max_addr = 0;

    /* --and read the bsp header for further loading progress (bsp header locates at the second sector of imagefile)*/
    block_read(block_device, (void*)sector_read_buff, image_loc + CONFIG_SECTOR_SIZE, CONFIG_SECTOR_SIZE);

    bsp_hdr_t* p_bsphdr = &g_bsp_hdr;

    /*Save the bsp header*/
    memcpy(p_bsphdr, sector_read_buff, sizeof(bsp_hdr_t));

    printf("=== Starting to load bsp ===\n");

    for (int i = 0; i < p_bsphdr->ph_nmbers; i++)
    {
        if(!is_valid_load_addr(p_bsphdr->prog_hdr[i].lma))
        {
            printf("Can't Load to invalid address %x,Please check the bsp program section %x \n", (paddr_t)p_bsphdr->prog_hdr[i].lma, i);
            return -1;
        }
        /* Find the max address bsp will occupy */
        if(bsp_load_max_addr < p_bsphdr->prog_hdr[i].lma + p_bsphdr->prog_hdr[i].mem_size)
        {
            bsp_load_max_addr = p_bsphdr->prog_hdr[i].lma + p_bsphdr->prog_hdr[i].mem_size;
        }
        if (bsp_load_min_addr > p_bsphdr->prog_hdr[i].lma)
        {
            bsp_load_min_addr = p_bsphdr->prog_hdr[i].lma;
        }
    }
    *p_image_loc = ALIGN_UP(bsp_load_min_addr, __PAGESIZE);

    load_program_sections(block_device, image_loc, p_bsphdr->prog_hdr, p_bsphdr->ph_nmbers, 0, 1);

    /* If we loaded all the program sections of bsp without mistake, --- */

    /* --- try to load the kernel --- */
#ifdef KIRK_LOAD_ALIGN
    uint64_t kernel_load_start_addr = ALIGN_UP(bsp_load_max_addr, KIRK_LOAD_ALIGN);
#else
    uint64_t kernel_load_start_addr = ALIGN_UP(bsp_load_max_addr, __PAGESIZE);
#endif
    uint64_t kernel_load_end_addr   = 0;

    uint64_t origin_kernel_load_minimal_addr = 0xFFFFFFFFFFFFFFFF;
    uint64_t origin_kernel_load_max_addr = 0;
    /* Read the kernel header */
    block_read(block_device, (void*)sector_read_buff, image_loc + p_imghdr->kernel_hdr_offset, CONFIG_SECTOR_SIZE);

    kernel_hdr_t* p_kerhdr = &g_kernel_hdr;

    /*Save the kernel header*/
    memcpy(p_kerhdr, sector_read_buff, sizeof(kernel_hdr_t));

    printf("=== Starting to load kernel ===\n");

    /* Find out the boot section and calculate the real entry of kernel */

    int64_t offset = 0;

    for(int i = 0; i < p_kerhdr->ph_nmbers; i++)
    {
        if(origin_kernel_load_minimal_addr > p_kerhdr->prog_hdr[i].lma)
        {
            origin_kernel_load_minimal_addr = p_kerhdr->prog_hdr[i].lma;
        }

        if(origin_kernel_load_max_addr < p_kerhdr->prog_hdr[i].lma + p_kerhdr->prog_hdr[i].mem_size)
        {
            origin_kernel_load_max_addr = p_kerhdr->prog_hdr[i].lma + p_kerhdr->prog_hdr[i].mem_size;
        }
    }

    offset = kernel_load_start_addr - origin_kernel_load_minimal_addr;

    // g_kernel_hdr.entry += offset;

    // printf("The kernel offset is %x,New entry relocated to %x\n", offset, g_kernel_hdr.entry);

    load_program_sections(block_device, image_loc, p_kerhdr->prog_hdr, p_kerhdr->ph_nmbers, offset, 1);

    kernel_load_end_addr = origin_kernel_load_max_addr + offset;

    /* Continue to load modules */
    application_hdr_t* p_modhdr;
    uint64_t module_load_start_addr = ALIGN_UP(kernel_load_end_addr, __PAGESIZE);

    for(int i = 0; i < p_imghdr->module_number; i++)
    {
        /* Read the module header */
        block_read(block_device, (void*)sector_read_buff, image_loc + p_imghdr->module_hdr_offset[i], CONFIG_SECTOR_SIZE);

        p_modhdr = &g_app_hdrs[i];

        memcpy(p_modhdr, sector_read_buff, sizeof(application_hdr_t));

        printf("=== Starting to load module[%d]:%s ===\n", (int)i, p_modhdr->name);

        /* When kernel load the modules, we don't care about the LMA, 
        so we use lma to indicate the position where the module was extracted in real ram */
        p_modhdr->prog_hdr[0].lma = module_load_start_addr;

        for(int j = 1; j < p_modhdr->ph_nmbers; j++)
        {
            p_modhdr->prog_hdr[j].lma = module_load_start_addr + p_modhdr->prog_hdr[j - 1].file_size;
            module_load_start_addr += p_modhdr->prog_hdr[j - 1].file_size;
        }

        /* Load the module's program sections with modified lma + offset(0) */
        load_program_sections(block_device, image_loc, p_modhdr->prog_hdr, p_modhdr->ph_nmbers, 0, 0);
        /* Ready for next section */
        module_load_start_addr = ALIGN_UP(module_load_start_addr + p_modhdr->prog_hdr[p_modhdr->ph_nmbers - 1].file_size, __PAGESIZE);
    }

    return module_load_start_addr;
}
