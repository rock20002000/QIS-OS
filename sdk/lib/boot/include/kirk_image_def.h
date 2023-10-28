#ifndef __KIRK_IMAGE_DEF_H__
#define __KIRK_IMAGE_DEF_H__


#include <stdint.h>
#include <stddef.h>


#define MAX_PATHNAME_LENTH              128

#define CONFIG_MAX_MODULES              50
#define CONFIG_MAX_KERNEL_SECTIONS      8
#define CONFIG_MAX_APP_SECTIONS         6
#define CONFIG_MAX_NAME_LENTH           64
#define CONFIG_MAX_APP_NAME_LENTH       128

#define CONFIG_SECTOR_SIZE              512
#define CONFIG_KIRKIMAGE_MAGIC          "kunyuimg"

typedef struct image_hdr
{
    //Magic code should be : 'kunyuimg' 
    char magic[8];

    char img_name[15];
    uint8_t module_number;
    uint64_t total_size;
    uint64_t kernel_hdr_offset;    
    uint64_t module_hdr_offset[CONFIG_MAX_MODULES];

    //Not used in this version,just remain it here
    uint32_t md5_code[4]; 

}__attribute__((packed)) image_hdr_t;

typedef struct program_hdr
{
    uint64_t offset;
    uint64_t vma;
    uint64_t lma;
    uint64_t file_size;
    uint64_t mem_size;
    uint64_t attr;
}__attribute__((packed)) program_hdr_t;

typedef struct kernel_hdr
{
    uint64_t entry;

    program_hdr_t prog_hdr[CONFIG_MAX_KERNEL_SECTIONS];
    uint8_t ph_nmbers;
    uint8_t reserved[7];
}__attribute__((packed)) kernel_hdr_t;

/*typedef struct kernel_hdr
{
    uint64_t entry;

    program_hdr_t prog_hdr[CONFIG_MAX_KERNEL_SECTIONS];
    uint8_t ph_nmbers;
}__attribute__((packed)) kernel_hdr_t;*/


//Bsp will share the kernel's header struct and can also use img_write_kernel to generate image
typedef kernel_hdr_t bsp_hdr_t;         


typedef struct application_hdr
{
    uint64_t entry;

    // Used to store flags,eg.:bit0 refers whether it is an auto-start program
    uint64_t app_attributes;
    program_hdr_t prog_hdr[CONFIG_MAX_APP_SECTIONS];
    uint8_t ph_nmbers;
    uint8_t reserved[7];
    char name[CONFIG_MAX_NAME_LENTH];	//	CONFIG_MAX_APP_NAME_LENTH];
}__attribute__((packed)) application_hdr_t;


#define EXTRABOOT_INFO_OFFSET           0x10
#define ORIGIN_EXTRA_BOOT_MAGIC         "origincode"
#define PROCESSED_EXTRA_BOOT_MAGIC      "kirk_boot"

typedef struct extraboot_info
{
    //Magic code should be : 'origincode' 
    char magic[16];

    uint64_t bootinfo_address;

}__attribute__((packed)) extraarch_boot_info_t;

#endif
