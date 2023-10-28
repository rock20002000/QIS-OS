#include <types.h>
#include <libc/include/ctype.h>
#include <libc/include/string.h>
#include <platform/include/compiler.h>
#include <platform/include/kirk_task.h>

extern char __cmdline[];
extern char __bss[];
extern char __bss_end[];

// for sparse
extern void malloc_init(void);
extern int main(int argc, char* argv[]);

const PROCESS_ENV g_procEnv __section(".procenv");

static int cmdline_init(char** p_arg, int args)
{
    int argc = 0;
#if 0   // strtok not OK!
    const char* delim = " \t";

    for (int i = 0; i < args && (*p_arg = strtok(__cmdline, delim)); i++)
    {
        argc++;
        xprintf("argv[%d] = %s\n", i, *p_arg);
        p_arg++;
    }
#else
    char* start = __cmdline;

    // xprintf("Process%d cmdline %p: %s\n", g_procEnv.pid, __cmdline, __cmdline);
    while (1)
    {
        while (*start && isblank(*start))
            start++;
        if (!*start)
            break;

        char* end = start + 1;

        while (*end && !isblank(*end))
            end++;

        *p_arg++ = start;
        argc++;

        start = end;
        if (!*start)
            break;
        *end = 0;
        start++;
    }
#endif
    return argc;
}

#define MAX_ARGS    32
static char* argv[MAX_ARGS];
void proc_entry(void)
{
    malloc_init();
    
    int argc = cmdline_init(argv, MAX_ARGS);

    int ret = main(argc, argv);

    exit();	// task_exit();
}
