#include <libc/include/malloc.h>
#include <platform/include/platform.h>
#include <libc/include/stdlib.h>

int main(int argc, char** argv)
{
    sysinfo_type_e type = SYSINFO_PROCESS;
    uintptr_t param = NULL_ID;
    size_t size = 0;
    int err = EOK;

#if 0
    if (argc > 1)
    {
        if (!strcmp(argv[1], "?") || !strcmp(argv[1], "-h"))
        {
            xprintf("Usage: %s [-v] [-m] [-p [pid]]\n", argv[0]);
            xprintf(" -v: show system version\n");
            xprintf(" -m: show system memory info\n");
            xprintf(" -p: show process info for special pid or all process\n");
            return err;
        }
        if ('-' == argv[1][0])
        {
            switch (argv[1][1])
            {
                case 'v':
                    type = SYSINFO_VERSION;
                    size = 64;
                break;
                case 'm':
                    type = SYSINFO_MEMORY;
                    size = sizeof(mem_info_t);
                break;
                case 'p':
                    type = SYSINFO_PROCESS;
                    if (argc > 2)
                    {
                        param = (uintptr_t)atoi(argv[2]);
                        if (NULL_ID != param)
                            size = sizeof(ps_info_t);
                    }
                break;
                #if 0
                case 's':
                    type = SYSINFO_SYSPAGE;
                break;
                #endif
            }
        }
    }
#else
    int opt;

    while ((opt = getopt(argc, argv, "hvmp:")) != -1)
    {
        if (opt == 'h')
        {
            xprintf("Usage: %s [-v] [-m] [-p [pid]]\n", argv[0]);
            xprintf(" -v: show system version\n");
            xprintf(" -m: show system memory info\n");
            xprintf(" -p: show process info for special pid or all process\n");
            return 0;
        }
        else if (opt == 'v')
        {
            type = SYSINFO_VERSION;
            size = 64;
        }
        else if (opt == 'm')
        {
            type = SYSINFO_MEMORY;
            size = sizeof(mem_info_t);
        }
        else if (opt == 'p')
        {
            type = SYSINFO_PROCESS;
            if (optarg)
            {
                param = (uintptr_t)atoi(argv[2]);                
                if (NULL_ID != param)
                    size = sizeof(ps_info_t);
            }
        }
    }
#endif
    if (!size)
    {
        err = GetSysInfo(type, param, NULL, 0);   // get sysinfo size
        if (IS_ERROR(err))
        {
            xprintf("%s failed: %d\n", argv[0], err);
            return err;
        }
        size = err;
    }

    void* p_sysinfo = malloc(size);

    if (!p_sysinfo)
    {
        xprintf("%s malloc(%d) failed!\n", argv[0], size);
        return ERR_NO_MEMORY;
    }

    err = GetSysInfo(type, param, p_sysinfo, size);
    if (err > 0)
    {
        if (SYSINFO_VERSION == type)
        {
            xprintf("----------------------------------------------------------------\n");
            xprintf("%s\n", (const char*)p_sysinfo);
            xprintf("----------------------------------------------------------------\n");
        }
        else if (SYSINFO_MEMORY == type)
        {
            mem_info_t* p_meminfo = (mem_info_t*)p_sysinfo;

            xprintf("----------------------------------------------------------------\n");
            xprintf("Total  pages: %d\n", p_meminfo->total_pages);
            xprintf("Free   pages: %d\n", p_meminfo->free_pages);
            xprintf("Shared pages: %d\n", p_meminfo->shared_pages);
            xprintf("Slob   pages: %d\n", p_meminfo->slob_pages);
            xprintf("----------------------------------------------------------------\n");
        }
        else if (SYSINFO_PROCESS == type)
        {
            int count = err / sizeof(ps_info_t);
            ps_info_t* ps_info = (ps_info_t*)p_sysinfo;

            xprintf("------------------------------------------------------------------------------------------------\n");
            xprintf("PID\tTID\tVSZ\tRSS\tCODE\tDATA\tSTAT\tPRIO\tTHREADS\tSTART\tTIME\tCOMMAND\n");

            for (int i =0; i < count; i++)
            {
                xprintf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%u\t%u\t%s\n",
                    ps_info[i].pid,
                    ps_info[i].tid,
                    ps_info[i].vsz,
                    ps_info[i].rss,
                    ps_info[i].code,
                    ps_info[i].data,
                    ps_info[i].stat,
                    ps_info[i].priority,
                    ps_info[i].threads,
                    ps_info[i].start,
                    ps_info[i].time,
                    ps_info[i].name);
            }
            xprintf("------------------------------------------------------------------------------------------------\n");
        }
    }
    else
    {
        xprintf("%s GetSysInfo failed: %d\n", argv[0], err);
    }

    free(p_sysinfo);
    return err;
}
