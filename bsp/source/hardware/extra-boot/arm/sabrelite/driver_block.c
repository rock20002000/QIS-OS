#include <types.h>
#include <libc/include/string.h>

/* We assume that our image is loaded into dram by 3rd-party bootloader(uboot),so just copy the data from ram position*/
int machine_block_read(int drive, void *buffer, uint64_t loc, size_t count)
{
    /* not used argument*/
    (void)drive;

    /* Read data from ram */
    memcpy(buffer, (void*)(size_t)loc, count);

    return 0;
}