#ifndef __ARM_CPU_COMMON_H__
#define __ARM_CPU_COMMON_H__

#include <arch.h>
#include <boot/include/bootinfo.h>
#include <boot/include/kirk_image_def.h>

void cpu_common_setup(syspage_t* p_syspage);

uint32_t get_random_long(void);

uint32_t get_random_int(void);

#endif
