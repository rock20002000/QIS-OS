#include <platform/include/compiler.h>
#include <platform/include/time.h>
#include <vdso.h>

notrace int __vdsoGetSystime(systime_t *st, const vdso_data_t *p_vdso_data, const void* p_vdso_text)
{
  if (!p_vdso_data || !st)
    return ERR_INVALID_ARG;

  st->ticks = p_vdso_data->ticks;

  if (p_vdso_text && p_vdso_data->systime.get_cycles)
  {
    st->cycles = ((size_t (*)(const struct vdso_systime*))((const char*)p_vdso_text + (size_t)(p_vdso_data->systime.get_cycles)))(&p_vdso_data->systime);
  }
  else
  {
    st->cycles = 0;
  }

	return EOK;
}

notrace size_t __vdsoGetCycles(const vdso_data_t *p_vdso_data, const void* p_vdso_text)
{
  size_t sys_cycles = 0;

  if (p_vdso_data)
  {
    sys_cycles = p_vdso_data->ticks * p_vdso_data->cycles_per_tick;
    if (p_vdso_text && p_vdso_data->systime.get_cycles)
    {
      sys_cycles += ((size_t (*)(const struct vdso_systime*))((const char*)p_vdso_text + (size_t)(p_vdso_data->systime.get_cycles)))(&p_vdso_data->systime);
    }
  }

	return sys_cycles;
}
