
#include <libc/include/string.h>

#include "debug.h"
#include "arch.h"
#include "kernel.h"
#include "ltimer.h"
#include "cpu_common.h"
#include <platform/arch/arm/include/mx6q/MCIMX6Y2.h>
//#include "intr/gic.h"
//#include "timer/gpt.h"



// callout functions
// static void setup_debug_device(void)
// {
//     bsp_device_init(g_syspage_ptr, DBG_DEV_INDEX);
// }


paddr_t early_calloc(size_t size, unsigned align)
{
    return g_syspage_ptr->early_alloc_func(&g_syspage_ptr->meminfo, size, align);
}


// Redefine weak function printchar to enable printf
void printchar(__unused struct vprintf_context *ctx, char ch) 
{
    bsp_device_t* debug_dev = bsp_get_device(g_syspage_ptr, DBG_DEV_INDEX);

    if (debug_dev)
    {
        void* fn_putc = debug_dev->interface[DBG_PUTC_INDEX];

        if (fn_putc)
        {
            if ('\n' == ch)
            {
                ((void (*)(bsp_device_t*, int))fn_putc)(debug_dev, '\r');
            }
            ((void (*)(bsp_device_t*, int))fn_putc)(debug_dev, ch);
        }
    }
}

extern void	init_traps(void);

extern void  _ker_main(void);

////////////////////
 Timer* p_newTimer= NULL;
extern list_t timersList;
 int TimerCallBackfn(void *arg)
{

  //  paddr_t pageTableAddress = arch_pte_backup();

    DBG("\n***********TimerCallBackfn ticks:%u***************\n",getTicks());
   // removeTimer(p_newTimer);
  //    list_nullify(&timersList);
   //arch_pte_restore(pageTableAddress);
    return 0;
}


extern volatile size_t gl_timer_tick;
void bootstrap_core_init(syspage_t* p_syspage, uint16_t cpu_idx)
{
	//	system_init_irq_table();
    //	system_init();

    gl_timer_tick = 0;

    // Set global syspage pointer
    g_syspage_ptr = p_syspage;
    arch_syspage_init(&g_syspage_ptr);

    // Set up debug device passed from bsp to enable debug functions
    // setup_debug_device();    // done in arch_syspage_init

	//printf("Testing BSP...\n");

    // Disable interrupts
    disable_interrupt();
  //  pic_init();


    // From now on, we can use print macros(based on printf) to show our debug information
    ASSERT(cpu_idx == 0);
    DBG("Welcome to kernel world! \n");

    // Save the bootinfo passed from bsp
    // store_bootinfo();

    // init memory
    mem_init(g_syspage_ptr);

    // Call init functions passed to syspage to get basic platform support
    cpu_common_setup(g_syspage_ptr);	//	arch_intr_init();	//    init_traps();
    
    //add Timer per 20 ticks
    initTimeList();

/*    p_newTimer= createTimer(TimerCallBackfn,20, NULL, 0);
    DBG(" p_newTimer->list:0x%p \n", &p_newTimer->list);
  //   DBG("CREATE SUCCESS timer->list:0x%p \n", &timer->list);
    addTimer(p_newTimer);
    DBG("%s \n",__func__);
*/
    // enable_interrupt();
	//arch_enable_irq(CONSOLE_IRQ);	//	ioapic_startup_vaddr, CONSOLE_IRQ);	//	serial_enable_interrupt();
	//arch_enable_irq(KEYBOARD_IRQ);	//	ioapic_startup_vaddr, KEYBOARD_IRQ);
	//arch_enable_irq(MOUSE_IRQ);		//	ioapic_startup_vaddr, MOUSE_IRQ);

	//arch_enable_irq(80);

    // Kick off our kirk kernel
    _ker_main();

    // Never run to here
    while(1)
    {

    }
}
