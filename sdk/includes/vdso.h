#ifndef __VDSO_H__
#define __VDSO_H__
#include <platform/include/time.h>

#define VDSO_SYSTIME_SYM   "__vdsoGetSystime"
#define VDSO_CYCLES_SYM    "__vdsoGetCycles"

struct vdso_systime {
   size_t   (*get_cycles)(const struct vdso_systime* p_systime);
   size_t   cycles_per_sec;
   size_t   timer_freq;
   size_t   boot_time;
   uint32_t intr;
   uint16_t code_size;
   uint16_t channel;
   size_t   st_size;
   addr_t   st_base;
};	// same as struct systime_entry
typedef struct
{
   size_t ticks;
   size_t cycles_per_tick;
   struct vdso_systime systime;
   systime_t interrupt_time;
   systime_t syscall_time;
} vdso_data_t;

#define ELF_MAGIC 0x464C457F

int __vdsoGetSystime(systime_t *st, const vdso_data_t *p_vdso_data, const void* p_vdso_text) ;
size_t __vdsoGetCycles(const vdso_data_t *p_vdso_data, const void* p_vdso_text);

#endif
