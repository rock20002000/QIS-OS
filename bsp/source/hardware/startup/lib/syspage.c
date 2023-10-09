#include <boot/include/bootinfo.h>
#include <libc/include/stdio.h>

#include "lib/startup.h"


/************************************
 * External functions 
 * 
 ************************************/

static void set_call_out_functions(syspage_t* p_syspage)
{
    p_syspage->early_alloc_func = calloc_ram;
}

void construct_syspage(syspage_t* p_syspage)
{
    // register call in/out functions
    // set_call_in_functions();
    set_call_out_functions(p_syspage);

    // Prepare pagetables for kernel
    arch_init_pgdir(p_syspage);

    arch_map_kernel(p_syspage->loadinfo);
}
