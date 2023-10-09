#include <types.h>

#include "../extra-boot/ex_boot.h"


int block_read(int drive, void *buffer, uint64_t loc, size_t count) 
{

    return machine_block_read(drive,buffer,loc,count);
}
