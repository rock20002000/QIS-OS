#include "debug.h"
#include "kernel.h"
#include "task.h"
#include "trap.h"
#include "arch_task.h"

extern void* kernel_stack_syscall_global[];
extern void* kernel_stack_interrupt_global[];

void userland_entry(void);	//	void arm_start_task(void);
void arm_task_switch(vaddr_t *prev_sp, vaddr_t next_sp);


//static uint32_t page_tables[50][512] __aligned(PAGE_SIZE);
//static uint8_t kernel_stacks[50][THREAD_STACK_SIZE_KERNEL] __aligned(
//		THREAD_STACK_SIZE_KERNEL);
//static uint8_t exception_stacks[50][THREAD_STACK_SIZE_KERNEL] __aligned(
//		THREAD_STACK_SIZE_KERNEL);


/*
static uint32_t page_tables[MAX_PROCESS_SUPPORT][512] __aligned(PAGE_SIZE);
static uint8_t kernel_stacks[MAX_PROCESS_SUPPORT][THREAD_STACK_SIZE_KERNEL] __aligned(
		THREAD_STACK_SIZE_KERNEL);
static uint8_t exception_stacks[MAX_PROCESS_SUPPORT][THREAD_STACK_SIZE_KERNEL] __aligned(
		THREAD_STACK_SIZE_KERNEL);
*/

// Prepare the initial stack for arm64_task_switch().
//static void init_stack(THREAD_t* thread, vaddr_t function_addr){	//	pc) {
//    // Initialize the page table.
//}


void arch_thread_init(THREAD_t* thread, uint64_t function_addr) {	//	void arch_thread_init_firstUserThread(THREAD_t* thread, uint64_t function_addr) {
    void* kernel_stack  = NULL;
    void* syscall_stack = NULL;

    kernel_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);    //16KB used for kernel stack
    if(NULL == kernel_stack)
    {
        goto exception_exit;
    }

    syscall_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);
    if(NULL == syscall_stack)
    {
        goto exception_exit;
    }

	//void *syscall_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);	//	(void *) kernel_stacks[thread->tid];
    //thread->arch.page_table = page_tables[thread->tid];

    thread->arch.interrupt_stack    = (vaddr_t)kernel_stack + THREAD_STACK_SIZE_KERNEL;
    thread->arch.syscall_stack      = (vaddr_t)syscall_stack + THREAD_STACK_SIZE_KERNEL;

    if(kernel_stack_interrupt_global[mp_self()]==NULL) kernel_stack_interrupt_global[mp_self()] = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM)  + THREAD_STACK_SIZE_KERNEL;	//thread->arch.interrupt_stack;
    if(kernel_stack_syscall_global[mp_self()]==NULL) kernel_stack_syscall_global[mp_self()] = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM)  + THREAD_STACK_SIZE_KERNEL;	//thread->arch.syscall_stack;

    //thread->arch.syscall_stack = (vaddr_t) syscall_stack + THREAD_STACK_SIZE_KERNEL;
    //	init_stack(thread, function_addr);	//	pc);


    //memset(thread->arch.page_table, 0, PAGE_SIZE);
    //thread->arch.ttbr0 = ptr2paddr(thread->arch.page_table);

    //vaddr_t exception_stack = (vaddr_t) exception_stacks[thread->tid];
    // uint32_t *sp = (uint32_t *) thread->arch.interrupt_stack;	//	(exception_stack + THREAD_STACK_SIZE_KERNEL);
    uint32_t *sp = (uint32_t *)(thread->arch.syscall_stack/* + THREAD_STACK_SIZE_KERNEL/2*/);
    // Fill the stack values for arm64_start_task().
    *--sp = (uint32_t)userland_entry;	//	pc;

    //	int num_zeroed_regs = 11;  // x19-x29
    //	for (int i = 0; i < num_zeroed_regs; i++) {
    //		*--sp = 0;
    //	}
    *--sp = (vaddr_t) function_addr;	//	arm_start_task;  // Task starts here (x30).

    thread->regs.sp = (vaddr_t) sp;		//	thread->arch.stack = (vaddr_t) sp;
    thread->regs.pc = function_addr;	//	The register is restored with interrupt frame, be with no business with thread stack


    thread->regs.spsr 	= 0x10;
    thread->regs.r0 	= 0x0;;
    thread->regs.r1 	= 0x0;;
    thread->regs.r2 	= 0x0;;
    thread->regs.r3 	= 0x0;;
    thread->regs.r4 	= 0x0;;
    thread->regs.r5 	= 0x0;;
    thread->regs.r6 	= 0x0;;
    thread->regs.r7 	= 0x0;;
    thread->regs.r8 	= 0x0;;
    thread->regs.r9 	= 0x0;;
    thread->regs.r10 	= 0x0;;
    thread->regs.r11 	= 0x0;;
    thread->regs.r12 	= 0x0;;
    thread->regs.lr 	= 0x0;;
    // KDBG("arch_thread_init: %p(%s) interrupt_stack:%p, syscall_stack:%p, sp:%p\n",
    //     thread, thread->name, thread->arch.interrupt_stack, thread->arch.syscall_stack, thread->regs.sp);
    return;

exception_exit:
	if (syscall_stack)
		kfree(syscall_stack);
	if (kernel_stack)
		kfree(kernel_stack);

}


/*
void arch_thread_init(THREAD_t* thread, uint64_t function_addr) {
    void* kernel_stack  = NULL;
    void* syscall_stack = NULL;

    kernel_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);    //16KB used for kernel stack
    if(NULL == kernel_stack)
    {
        goto exception_exit;
    }

    syscall_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);
    if(NULL == syscall_stack)
    {
        goto exception_exit;
    }

    thread->arch.interrupt_stack    = (uint64_t)kernel_stack + THREAD_STACK_SIZE_KERNEL;
    thread->arch.syscall_stack      = (uint64_t)syscall_stack + THREAD_STACK_SIZE_KERNEL;

    uint32_t *sp = (uint32_t *) thread->arch.interrupt_stack;	//	(exception_stack + THREAD_STACK_SIZE_KERNEL);

    thread->regs.sp = (vaddr_t) sp;		//	thread->arch.stack = (vaddr_t) sp;
    thread->regs.pc = function_addr;	//	The register is restored with interrupt frame, be with no business with thread stack

    thread->regs.spsr 	= 0x10;
    thread->regs.r0 	= 0x0;;
    thread->regs.r1 	= 0x0;;
    thread->regs.r2 	= 0x0;;
    thread->regs.r3 	= 0x0;;
    thread->regs.r4 	= 0x0;;
    thread->regs.r5 	= 0x0;;
    thread->regs.r6 	= 0x0;;
    thread->regs.r7 	= 0x0;;
    thread->regs.r8 	= 0x0;;
    thread->regs.r9 	= 0x0;;
    thread->regs.r10 	= 0x0;;
    thread->regs.r11 	= 0x0;;
    thread->regs.r12 	= 0x0;;
    thread->regs.lr 	= 0x0;;


exception_exit:
	if (syscall_stack)
		kfree(syscall_stack);
	if (kernel_stack)
		kfree(kernel_stack);

}
*/

void arch_thread_init_withParam(THREAD_t* thread, uint64_t function_addr,
                                void* threadParams, void* extParam,
                                THREAD_t* pCurThread) {
    void* kernel_stack  = NULL;
    void* syscall_stack = NULL;

    kernel_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);    //16KB used for kernel stack
    if(NULL == kernel_stack)
    {
        goto exception_exit;
    }

    syscall_stack = kmalloc(THREAD_STACK_SIZE_KERNEL, KMEM);
    if(NULL == syscall_stack)
    {
        goto exception_exit;
    }

    thread->arch.interrupt_stack    = (vaddr_t)kernel_stack + THREAD_STACK_SIZE_KERNEL;
    thread->arch.syscall_stack      = (vaddr_t)syscall_stack + THREAD_STACK_SIZE_KERNEL;

    if (thread->is_user_thread)
    {
        size_t size = thread_stack_size(thread);
        vaddr_t stack_addr = vmalloc(pCurThread->process, NULL_VADDR, &size, MAP_FLAGS_STACK);

        if (NULL_VADDR == stack_addr)
        {
            goto exception_exit;
        }
        thread->regs.sp = stack_addr + size;
        //	thread->regs.rsp = thread->regs.rbp = thread->arch.rsp = thread->arch.rsp = stack_addr + size;
    }

    thread->regs.pc = (vaddr_t) function_addr;

    thread->regs.r0 = (vaddr_t) threadParams;
    thread->regs.r1 = (vaddr_t) extParam;


    thread->regs.spsr 	= 0x10;
    thread->regs.r2 	= 0x0;;
    thread->regs.r3 	= 0x0;;
    thread->regs.r4 	= 0x0;;
    thread->regs.r5 	= 0x0;;
    thread->regs.r6 	= 0x0;;
    thread->regs.r7 	= 0x0;;
    thread->regs.r8 	= 0x0;;
    thread->regs.r9 	= 0x0;;
    thread->regs.r10 	= 0x0;;
    thread->regs.r11 	= 0x0;;
    thread->regs.r12 	= 0x0;;
    thread->regs.lr 	= 0x0;;
    // KDBG("arch_thread_init_withParam: %p(%s) interrupt_stack:%p, syscall_stack:%p, sp:%p\n",
    //     thread, thread->name, thread->arch.interrupt_stack, thread->arch.syscall_stack, thread->regs.sp);
    return;
exception_exit:
	if (syscall_stack)
		kfree(syscall_stack);
	if (kernel_stack)
		kfree(kernel_stack);
}

/*
    void *syscall_stack = (void *) kernel_stacks[thread->tid];
    //thread->arch.page_table = page_tables[thread->tid];
    thread->arch.syscall_stack = (vaddr_t) syscall_stack + THREAD_STACK_SIZE_KERNEL;


    thread->process = pCurThread->process;


    //init_stack_withParam(thread, function_addr);	//	pc);



    //memset(thread->arch.page_table, 0, PAGE_SIZE);
    //thread->arch.ttbr0 = ptr2paddr(thread->arch.page_table);

    vaddr_t exception_stack = (vaddr_t) exception_stacks[thread->tid];
    uint64_t *sp = (uint64_t *) (exception_stack + THREAD_STACK_SIZE_KERNEL);
    // Fill the stack values for arm64_start_task().
    *--sp = function_addr;	//	pc;

    int num_zeroed_regs = 11;  // x19-x29
    for (int i = 0; i < num_zeroed_regs; i++) {
        *--sp = 0;
    }
    *--sp = (vaddr_t) userland_entry;	//	arm_start_task;  // Task starts here (x30).

    //	thread->arch.stack = (vaddr_t) sp;
 */


void arch_task_switch(THREAD_t* prev, THREAD_t* next) {
	ARCH_CPUVAR->esp1 = next->arch.syscall_stack;
	kernel_stack_syscall_global[mp_self()] = (void*)next->arch.syscall_stack;
	kernel_stack_interrupt_global[mp_self()] = (void*)next->arch.interrupt_stack;
    // KDBG("arch_task_switch: prev(%s) sp:%p, next(%s) sp:%p, sp: %p\n",
    //     prev->name, prev->regs.sp, next->name, next->regs.sp, arm_stack());
	arch_pte_write(arch_pte_get_entry(next->process->ms.mmu));
    arm_task_switch((vaddr_t*)(&prev->regs.sp), next->regs.sp);	//	(&prev->arch.stack, next->arch.stack);
}

void arch_task_switch_intr(THREAD_t* prev, THREAD_t* next, iframe_t* iframe) {
    paddr_t next_pgTableAddr = 0;
    // Disable interrupts in case they're not yet disabled.
    //	asm_cli();
    // Update user GS base.
    //	asm_swapgs();
    //prev->process->arch.gsbase = asm_rdgsbase();
    //asm_wrgsbase(next->process->arch.gsbase);
    //asm_swapgs();

    if(0 == next->is_user_thread)
    {
        PANIC("arch_task_switch_intr from %p:%s to kernel thread", prev, prev->name);
    	next_pgTableAddr = system_pte(-1);	//	(uint64_t)__kernel_pml4;
    }
    else
    {
    	next_pgTableAddr = arch_pte_get_entry(next->process->ms.mmu);
    }


    // Update the kernel stack for syscall and interrupt/exception handlers.
    ARCH_CPUVAR->esp1 = next->arch.syscall_stack;
    kernel_stack_syscall_global[mp_self()] = (void*)next->arch.syscall_stack;
    kernel_stack_interrupt_global[mp_self()] = (void*)next->arch.interrupt_stack;
    //ARCH_CPUVAR->tss.rsp0 = next->arch.interrupt_stack;

    {
    	saveRegsFrame(iframe, &(next->regs));
    	/*	iframe->r0		= next->regs.r0	;		//	uint64_t
    	iframe->r1		= next->regs.r1	;		//	uint64_t
    	iframe->r2		= next->regs.r2	;		//	uint64_t
    	iframe->r3		= next->regs.r3	;		//	uint64_t
    	iframe->r4		= next->regs.r4	;		//	uint64_t
    	iframe->r5		= next->regs.r5	;		//	uint64_t
    	iframe->r6		= next->regs.r6	;		//	uint64_t
    	iframe->r7		= next->regs.r7	;		//	uint64_t
    	iframe->r8		= next->regs.r8	;		//	uint64_t
    	iframe->r9		= next->regs.r9	;		//	uint64_t
    	iframe->r10		= next->regs.r10	;		//	uint64_t
    	iframe->r11		= next->regs.r11	;		//	uint64_t
    	iframe->r12		= next->regs.r12	;		//	uint64_t
    	iframe->sp		= next->regs.sp	;		//	uint64_t
    	iframe->lr		= next->regs.lr	;		//	uint64_t
    	iframe->pc		= next->regs.pc	;		//	uint64_t
    	//	iframe->cpsr	= next->regs.cpsr	;		//	uint64_t

    	 */
    }

    next->quantum = 0;
    //if( (prev->process != next->process) || (prev->is_user_thread != next->is_user_thread) )
    {
        // Switch the page table.
    	//DBG("[arch_task_switch2] Switch the page table %p for thread:%d ... \n", next_pml4, next->tid);
        arch_pte_write(next_pgTableAddr);
    }
}

void arch_thread_destroy(THREAD_t* thread) {}
