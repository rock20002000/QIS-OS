#include <platform/include/platform.h>
#include <platform/include/syscalls.h>
#include <elf_vDSO.h>
#include <vdso.h>

#define VDSO_CYCLES_TEST

#ifdef __LP64__
 extern void* __vdsosym(const char *name);
 #define vdsoFindSymbol __vdsosym
#else
 extern void* __vdsosym32(const char *name);
 #define vdsoFindSymbol __vdsosym32
#endif
static size_t vdsoGetCycles(void)
{
    static size_t (*get_cycles)(const vdso_data_t*, const void*) = NULL;

    if (!get_cycles)
    {
        get_cycles = vdsoFindSymbol(VDSO_CYCLES_SYM);
        if (!get_cycles)
        {
            xprintf("vdsoFindSymbol(%s) failed!\n", VDSO_CYCLES_SYM);
            return 0;
        }
    }
    return get_cycles((const vdso_data_t*)g_procEnv.vdso_data, (const void*)g_procEnv.vdso_text);
}

static int vdsoGetSystime(struct timespec* tp)
{
    systime_t st;
    static size_t (*get_systime)(systime_t*, const vdso_data_t*, const void*) = NULL;

    if (!g_procEnv.systimer_freq)
        return ERR_NOT_SUPPORT;

    if (!get_systime)
    {
        get_systime = vdsoFindSymbol(VDSO_SYSTIME_SYM);
        if (!get_systime)
        {
            xprintf("vdsoFindSymbol(%s) failed!\n", VDSO_SYSTIME_SYM);
            return 0;
        }
    }

    int err = get_systime(&st, (const vdso_data_t*)g_procEnv.vdso_data, (const void*)g_procEnv.vdso_text);

    if (IS_ERROR(err))
        return err;

	size_t tv_sec = st.ticks / g_procEnv.systimer_freq;
	size_t tv_nsec = (st.ticks % g_procEnv.systimer_freq) * (1000000000 / g_procEnv.systimer_freq);

	if (st.cycles)
	{
		size_t cycles_per_time = g_procEnv.cycles_per_sec / g_procEnv.systimer_freq;

		tv_nsec += st.cycles * (1000000000 / g_procEnv.systimer_freq) / cycles_per_time;
		if (tv_nsec >= 1000000000)
		{
			tv_nsec -= 1000000000;
			tv_sec++;
		}
	}

	if (tp)
	{
		tp->tv_sec = tv_sec;
		tp->tv_nsec = tv_nsec;
	}

    return EOK;
}

static bool vdsoTestCycles(void)
{
    size_t cycles1 = vdsoGetCycles();

    if (!cycles1)
        return false;

    msleep(10);

    size_t cycles2 = vdsoGetCycles();

    if (cycles2 <= cycles1)
        return false;
    xprintf("vdsoGetCycles: %u, %u\n", cycles1, cycles2);
    return true;
}

static bool vdsoTestSystime(void)
{
    struct timespec ts;

    if (IS_ERROR(vdsoGetSystime(&ts)))
        return false;

    xprintf("vdsoGetSystime: %u.%u\n", ts.tv_sec, ts.tv_nsec);
    return true;
}

void main(int argc, char* argv[])
{
    bool ret = true;
    const char* vdso_func_name = VDSO_SYSTIME_SYM;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "-t"))
            vdso_func_name = VDSO_SYSTIME_SYM;
        else if (!strcmp(argv[1], "-c"))
            vdso_func_name = VDSO_CYCLES_SYM;
        else if (!strcmp(argv[1], "-h"))
        {
            xprintf("Usage: %s [-c] [-s]\n", argv[0]);
            xprintf(" -c: get cycles\n -s: get systime\n");
            return EOK;
        }
    }

    xprintf("vdso test: %s\n", vdso_func_name);

    if (!strcmp(vdso_func_name, VDSO_SYSTIME_SYM))
        ret = vdsoTestSystime();
    else
        ret = vdsoTestCycles();

    return ret ? EOK : ERR_ABORTED;
}
