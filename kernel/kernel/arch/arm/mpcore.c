#include <libc/include/string.h>
#include "debug.h"
#include "arch.h"
#include "kernel.h"
#include "ltimer.h"
#include "cpu_common.h"
#include "smp32.h"
#include <types.h>
#include <platform/arch/arm/include/mx6q/MCIMX6Y2.h>
#include <boot/include/bootinfo.h>
smp_entry_t smp_entry;

size_t max_cpus = 128;

vaddr_t get_scu()
{
    unsigned scu;

    __asm__ __volatile__("mrc p15, 4, %0, c15, c0, 0":"=r"(scu));

    mpcore_scu_base = scu;
    return scu;
}

void smp_spin(smp_entry_t* smp_entry, unsigned int cpuidx );

void(*smp_spin_vaddr)(smp_entry_t* smpEntry, unsigned int cpuidx);

void init_smp(syspage_t*  p_sysconfig){
    int cpu_num;

    get_scu();

    if(max_cpus > 0) {
        cpu_num = board_smp_num_cpu();
        if ( cpu_num > max_cpus) cpu_num = max_cpus;
        p_sysconfig->num_cpu = cpu_num;
        //smp_spin_vaddr = smp_spin;
        //smp_spin(&smp_entry, 0);

    }

    return ;

}