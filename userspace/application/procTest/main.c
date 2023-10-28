#include <libc/include/string.h>
#include <platform/include/platform.h>
#include <utils/include/ringbuf.h>
#include <fcntl.h>

#define MMAP_TEST

#ifdef MMAP_TEST
#include <libc/include/malloc.h>
 #define ORDERS         17
 #define MIN_SIZE       64
 #define TEST_COUNT     16
#endif

int main(int argc, char* argv[])
{
    xprintf("------------------------ proc%d: %s ------------------------\n", g_procEnv.pid, argv[0]);
#ifdef MMAP_TEST
    xprintf(" vdso_data: %p, vdso_text: %p\n", g_procEnv.vdso_data, g_procEnv.vdso_text);

    void* ptrs[ORDERS];

    const uint64_t* vd = (const uint64_t*)(g_procEnv.vdso_data + 0x78);
    const uint64_t* vt = (const uint64_t*)(g_procEnv.vdso_text + 0x90);
    
    xprintf("dump vdso_data: %p at %p, vdso_text: %p at %p\n", *vd, vd, *vt, vt);
   
    xprintf("heap base: %p\n", heap_base());
    for (int i = 0; i < TEST_COUNT; i++)
    {
        size_t size = MIN_SIZE;

        for (int j = 0; j < ORDERS; j++)
            ptrs[j] = NULL;
        xprintf("mmap/mmumap test %d malloc...\n", i);
        for (int j = 0; j < ORDERS; j++)
        {
            ptrs[j] = malloc(size);
            if (!ptrs[j])
                xprintf("malloc(%x) failed\n", size);
            else
            {
                sprintf((char *)ptrs[j], " mmap%d, malloc(%x) test data at %x\n", i, size, ptrs[j]);
            }
            size <<= 1;
        }
        xprintf("mmap/mmumap test %d free...\n", i);
        for (int j = 0; j < ORDERS; j++)
        {
            if (ptrs[j])
            {
                xprintf("%s", (char *)ptrs[j]);
                free(ptrs[j]);
            }
        }
    }

    xprintf("access memory after mmumap ...\n");
    for (int j = 0; j < ORDERS; j++)
    {
        if (ptrs[j])
            xprintf("%s", (char *)ptrs[j]);
    }
    xprintf("-------------------------------- malloc test over ----------------\n");
#else
#endif
    return EOK;
}
