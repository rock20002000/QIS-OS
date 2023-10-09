
#include <types.h>
#include <boot/include/bootinfo.h>
#include <libc/include/stdio.h>
#include <libc/include/string.h>

#include "debug.h"
#include "aether_resmgr.h"
#include "task.h"
#include "kernel.h"

syspage_t* g_syspage_ptr;
const char* g_version;
extern PROCESS_t g_ker_process;

volatile size_t gl_timer_tick;
struct tm       g_initBiosTime;

size_t getTicks(void)
{
   return gl_timer_tick;
}

// External functions
extern void dbg_module_test(void);

extern void put_vga(int c);
void screen_printf(char* pszTmp){
	while((*pszTmp)!='\0'){
		put_vga(*pszTmp);	//	x64_screen_printchar(*pszTmp);
		pszTmp++;
	}
	return;
}

//void gen_softintr(){
//	asm volatile("movl $1, %%eax \n int $0x80"::);
//	//	asm volatile("int $0x3 "::);
//}

void test_idle_thread(void)
{
    while(1);
}
void test_thread(void)
{
    while(1){
    	printf("Test thread...\n");
    }
}

uint32_t phy_interrupt_from_thread;
uint32_t phy_interrupt_to_thread;
uint32_t phy_thread_switch_interrupt_flag;

  void phy_hw_interrupt_init(void)
  {
    //phy_hw_vector_init();
    //  gic_init();
  
      /* init interrupt nest, and context in thread sp */
   //   phy_interrupt_nest = 0;
      phy_interrupt_from_thread = 0;
      phy_interrupt_to_thread = 0;
      phy_thread_switch_interrupt_flag = 0;
  }

static char kernel_version[32];
void _ker_main(void) 
{
#define DEF2STR(x)  #x
#define VER_STR(x)  DEF2STR(x)
	// char szTmp[1024] = {0};

    //	dbg_module_test();

    phy_hw_interrupt_init();

	get_biostime(&g_initBiosTime);
	// KDBG("[%s %d] tick:%d\n",__func__, __LINE__, getTicks());
    sprintf(kernel_version, "QIS 0.11 v%d.%d.%d r%s", MAJOR_VERSION, MINOR_VERSION, BUILD_NO, VER_STR(GIT_VERSION));
    g_version = kernel_version;
	KINFO("Welcome to use %s build at %s %s\n", kernel_version, __DATE__, __TIME__);
	// sprintf(szTmp, "Welcome to use QIS 0.11 version 0.1 build %d-%d-%d %d:%d:%d \n", g_initBiosTime.tm_year, g_initBiosTime.tm_mon, g_initBiosTime.tm_mday, g_initBiosTime.tm_hour, g_initBiosTime.tm_min, g_initBiosTime.tm_sec );
	// screen_printf(szTmp);

    vmm_init(&g_ker_process, g_syspage_ptr);

    pathmgr_init();

    scheduler_init();
    resmgr_init();

    ////////////////////////////////////////////////////////////////////////////
    /*cpuvar_ptr cpu_var = get_cpuvar();
    init_thread(&cpu_var->idle_task, "idle", test_idle_thread, MAX_PRIORITYS - 1, false);

    THREAD_t* prev = CURRENT_THREAD;
    THREAD_t* next = init_thread(NULL, "Test", test_thread, 10 ,true);	//	thread_runqueue_pop();
    if(NULL == prev){
    	prev = IDLE_THREAD;
    }
    arch_task_switch(prev, next);*/
    ////////////////////////////////////////////////////////////////////////////

    init_proc();	//	init process with index 0

    init_taskmgr(g_syspage_ptr);

    //gen_softintr();

    //enable_interrupt();
    start_sched();
    //	enable_interrupt();

    while(1)
    {
        /*	if((gl_timer_tick%1000)==0){
        	char szTmp[1024] = {0};
        	struct tm   curBiosTime;
        	get_biostime(&curBiosTime);
        	sprintf(szTmp, "Current time: %d-%d-%d %d:%d:%d %d\n", curBiosTime.tm_year, curBiosTime.tm_mon, curBiosTime.tm_mday, curBiosTime.tm_hour, curBiosTime.tm_min, curBiosTime.tm_sec, gl_timer_tick );
        	screen_printf(szTmp);
        }	*/
        //	DBG("Current tick is %d!\n",gl_timer_count);
    }
}

void mpinit(void)
{
    while(1)
    {
    }
}
