#include <platform/include/platform.h>
#include <elf_vDSO.h>
#include <vdso.h>

#define VDSO_TEST

#ifdef VDSO_TEST
#ifdef __LP64__
 extern void* __vdsosym(const char *name);
 #define vdsoFindSymbol __vdsosym
#else
 extern void* __vdsosym32(const char *name);
 #define vdsoFindSymbol __vdsosym32
#endif
static void vdsoTest(void)
{
#define ELF_MAGIC 0x464C457F
    vaddr_t funp2, funp3;
    vaddr_t pVdsoStart = g_procEnv.vdso_text;
   
     uint32_t *elf_magic = (uint32_t *) pVdsoStart;
     if (*elf_magic != ELF_MAGIC) {
        xprintf("elf magic bad\n");
        return ERR_UNAVAILABLE;
    }
     xprintf("%s elf magic 0x%p\n",__func__, *elf_magic);

    const vdso_data_t *pVdsoData = (const vdso_data_t *)(paddr_t)g_procEnv.vdso_data;

    funp2 =  vdsoFindSymbol(VDSO_SYSTIME_SYM);
    xprintf("[%s:%d] %s: %p\n", __func__, __LINE__, VDSO_SYSTIME_SYM ,funp2 );
    int (*f)(systime_t*, const vdso_data_t*, const void*) =
        (int (*)(systime_t*, const vdso_data_t*, const void*))funp2;
    systime_t st;  
 	int ret = f(&st, pVdsoData, (const void*)g_procEnv.vdso_text);
    xprintf("\n%s(),  time: %d.%d\n", VDSO_SYSTIME_SYM, st.ticks, st.cycles);

    funp3=vdsoFindSymbol(VDSO_CYCLES_SYM);
    xprintf("[%s:%d] %s: %p \n", __func__, __LINE__, VDSO_CYCLES_SYM ,funp3);
    size_t (*fn)(const vdso_data_t*, const void*) =
 		(size_t (*)(const vdso_data_t*, const void*))funp3;
    size_t cycles = fn(pVdsoData, (const void*)g_procEnv.vdso_text);
    xprintf("\n%s(), cycles: %d \n", VDSO_CYCLES_SYM, cycles);
}
#endif

void main(int argc, char* argv[])
{
 #ifdef VDSO_TEST
    xprintf("VDSO32 TEST. \n");
    vdsoTest();
 #endif
 #ifdef RBTREE_TEST
    extern int rbmain();
    rbmain();
 #endif
 #ifdef SEMAPHORE_TEST
    extern void semaphoreTest();
    semaphoreTest();
 #endif
    return EOK;
}
