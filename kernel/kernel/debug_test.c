//In this file,we can build out test routine for different modules!
#include <boot/include/bootinfo.h>
#include <libc/include/string.h>

#include "debug.h"
#include "kernel.h"

// /***************************************************************/
// #include "id_pool.h"
// id_pool_t test_id,test_id2;

// void id_pool_test(void)
// {
//     id_pool_init(&test_id, 0x2000);

//     DBG("Initialized  test id: \n  bitmap_addr: 0x%p\n  size:      %d",test_id.bitmap,test_id.size);

//     id_pool_init(&test_id2, 0x4000);

//     DBG("Initialized  test id2: \n  bitmap_addr: 0x%p\n  size:      %d",test_id2.bitmap,test_id2.size);

//     size_t id0 = id_pool_allocate_id(&test_id);

//     size_t id1 = id_pool_allocate_id(&test_id);

//     DBG("id0 = %d,id1 = %d \n",id0,id1);

//     id_pool_free_id(&test_id,1);

//     size_t id2 = id_pool_allocate_id(&test_id);

//     DBG("id2 = %d \n",id2);
// }

// /***************************************************************/
// #include "dyn_array.h"

// struct my_struct
// {
//     size_t haha;
//     size_t heihei;
//     size_t hoho;
//     size_t lala;
// };

// DYNARRAY_STRUCT(struct my_struct *)dynarry_test;
// DYNARRAY_STRUCT(size_t)xx_val;

// void dyn_array_test(void)
// {
//     struct my_struct *xxx;
//     struct my_struct *yyy;
//     void *pool = kmalloc(sizeof(struct my_struct)*2);

//     xxx = pool;
//     yyy = pool + sizeof(struct my_struct);

//     int i = 0;
//     xxx->haha= 111;
//     xxx->heihei = 222;
//     xxx->hoho = 333;
//     xxx->lala = 444;
//     i = DYNARRAY_INSERT(dynarry_test, xxx);
//     DBG("i = %d \n",i);
//     yyy->haha = 777;
//     i = DYNARRAY_INSERT(dynarry_test, yyy);
//     DBG("i = %d \n",i);

//     i = DYNARRAY_REMOVE_BY_VALUE_AND_PACK(dynarry_test, xxx);
//     DBG("i = %d \n",i);

//     // size_t aa = 0x555,bb= 0x888;
//     // DYNARRAY_INSERT(xx_val, aa);
//     // DYNARRAY_INSERT(xx_val, bb);
// }


// /***************************************************************/
// #include "arch_mm.h"
// struct arch_proc proc_archx;
// struct arch_proc proc_arch2;
// void vm_map_test()
// {
    
//     proc_archx.mmu.pml4 = (paddr_t)pmm_allocz(1);
//     proc_arch2.mmu.pml4 = (paddr_t)pmm_allocz(1);

//     paddr_t test_paddr = (paddr_t)pmm_alloc(1);
//     paddr_t test_paddr2 = (paddr_t)pmm_alloc(1);

//     pte_map_page(&proc_archx.mmu, 0x123456789, test_paddr, 0b10);
//     pte_map_page(&proc_arch2.mmu, 0x123456789, test_paddr2, 0b10);

//     paddr_t paddr_xx = arch_vm_v2p_addr(&proc_archx.mmu, 0x123456789);

//     DBG("vm :0x123456789 in process 1 has a phisical address : 0x%p\n",paddr_xx);

//     paddr_xx = arch_vm_v2p_addr(&proc_arch2.mmu, 0x123456789);

//     DBG("vm :0x123456789 in process 2 has a phisical address : 0x%p\n",paddr_xx);
// }

// /***************************************************************/

// void cpu_var_test()
// {
//     cpuvar_ptr cpu_var = get_cpuvar();
//     uint64_t tss_addr = (uint64_t) &ARCH_CPUVAR->tss;
//     struct gdt *gdt = &ARCH_CPUVAR->gdt;
//     struct idt *idt = &ARCH_CPUVAR->idt;

//     DBG("Show CPU info here:\n\
//      CPUID      = %d\n\
//      GDT addr   = 0x%p\n\
//      IDT addr   = 0X%p\n\
//      TSS addr   = 0X%p\n",
//      cpu_var->cpu_id,
//      gdt,
//      idt,
//      tss_addr);
// }


/***************************************************************/
extern paddr_t early_calloc(size_t size, unsigned align);


static void print_ramlist(void)
{
    for(int i = 0; i < g_syspage_ptr->meminfo.ram_list_size; i++)
    {
        printf("Ramlist %d: start from %p, end with %p \n",i, g_syspage_ptr->meminfo.ram_list[i].addr, 
                g_syspage_ptr->meminfo.ram_list[i].addr + g_syspage_ptr->meminfo.ram_list[i].size);
    }

}

static void test_early_alloc(void)
{
    DBG("Before we calloc: \n");
    print_ramlist();


    paddr_t ret = early_calloc(51, 4);
    DBG("Allocated a 51 bytes ram at %p,ramlist changes: \n", ret);
    print_ramlist();   

    ret = early_calloc(100, 4096);
    DBG("Allocated a 100 bytes ram aligned with PAGESIZE at %p,ramlist changes: \n", ret);
    print_ramlist();
}

/***************************************************************/
static void test_alloc(void)
{
    paddr_t ret = (paddr_t)kmalloc(PAGE_SIZE, 0);
    DBG("Allocated a 4k bytes ram at %p \n", ret);

    ret = (paddr_t)kmalloc(PAGE_SIZE*10, 0);
    DBG("Allocated a 40k bytes ram at %p \n", ret);

    // ret = (paddr_t)realloc((void*)ret, PAGE_SIZE*8);
    // DBG("Resize the 40k bytes ram to 32K with new address %p \n", ret);   

    ret = (paddr_t)kmalloc(512, 0);
    DBG("Allocated a 512 bytes ram at %p \n", ret);
}

/***************************************************************/
static void print_test(void)
{
    ;	//	unsigned offset = offsetof(struct cpuvar, cur_task_pgdir);
    //	printf("cur_task_pgdir offset in cpuvar is %p\n", offset);
}

/***************************************************************/
paddr_t palloc_test(size_t count)
{
    paddr_t paddr = alloc_pages(count, 0);

    if (paddr == NULL_PADDR)
    {
        PANIC("alloc_pages %u pages failed!", count);
        return NULL_PADDR;
    }

    uint32_t bytes = count << PAGE_SHIFT;
    uint32_t *p = paddr2ptr(paddr), tp = (uint32_t)paddr;

    for (int i = 0; i < bytes; i += sizeof(*p))
    {
        *p++ = tp++;
    }
    return paddr;
}
uint32_t pfree_test(paddr_t paddr, uint32_t count)
{
    uint32_t bytes = count << PAGE_SHIFT;
    uint32_t *p = paddr2ptr(paddr), tp = (uint32_t)paddr;

    for (int i = 0; i < bytes; i += sizeof(*p))
    {
        if (*p != tp)
        {
            WARN("pfree_test(%p, %u) + 0x%x", paddr, count, i);
            WARN("data error at: %p, 0x%x != 0x%x", p, *p, tp);
            break;
        }
        p++;
        tp++;
    }
    return free_pages(paddr);
}

#define MTEST_COUNT 16
void mem_test(void)
{
    paddr_t paddr[MTEST_COUNT+1];

    mem_info();
    for (int i = MTEST_COUNT; i <= MTEST_COUNT; i++)
    {
        paddr[i] = palloc_test(1 << i);
    }
    for (int i = MTEST_COUNT; i >= MTEST_COUNT; i--)
    {
        if (NULL_PADDR != paddr[i])
        {
            pfree_test(paddr[i], 1 << i);
        }
    }
    mem_info();
}
/***************************************************************/

/***************************************************************/
void dbg_module_test(void)
{
    if(0)
    {
        // id_pool_test();
        // dyn_array_test();
        // cpu_var_test();
        // vm_map_test();
        test_early_alloc();
        test_alloc();
        print_test();
        mem_test();
    }
}
