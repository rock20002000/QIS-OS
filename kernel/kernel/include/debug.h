#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__
#include <types.h>
#include <libc/include/stdio.h>
#include <utils/include/print_macros.h>
#include <arch.h>
//#include <../arch/x86_64/include/arch.h>
/*
static inline void KDBG(fmt){
    do {
        paddr_t arch_pte_backup();                                                                       \
        printf(SGR_DEBUG "[%s]:%d \n" fmt SGR_RESET "\n",__FILE__, __LINE__,  ##__VA_ARGS__);
        arch_pte_restore(pageTableAddress);
    } while (0);
}
*/
#endif
