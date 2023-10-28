#include <libc/include/string.h>

#include "kernel.h"
#include "task.h"
#include "arch_task.h"

extern DYNARRAY_STRUCT(THREAD_t *) 	thread_fifo_running_lists[];		//	The highest priority system blocking threads to be run
extern DYNARRAY_STRUCT(THREAD_t *) 	thread_fifo_waiting_lists[];		//	The highest priority system blocking threads to be run

extern DYNARRAY_STRUCT(THREAD_t *) 	thread_privilege_lists[];	//	The privileged threads to be run

extern DYNARRAY_STRUCT(THREAD_t *) 	thread_runing_lists[];		//	The normal running threads

extern DYNARRAY_STRUCT(THREAD_t *) 	thread_waiting_lists[];		//	The threads just created waiting to be scheduled to run

extern DYNARRAY_STRUCT(THREAD_t *) 	thread_destroy_lists[];


//	extern DYNARRAY_STRUCT(PROCESS_t *) process_created_queue;		//	The processes created

extern DYNARRAY_STRUCT(PROCESS_t *) process_vector;				//	The processes created

extern DYNARRAY_STRUCT(PROCESS_t *) process_lookup_lists[];		//	The processes look up list with page table address


void scheduler_init()
{
    for(int i = 0; i < MAX_PRIORITYS; i++)
    {
		DYNARRAY_NULLIFY(thread_fifo_running_lists[i]);
		DYNARRAY_NULLIFY(thread_fifo_waiting_lists[i]);

		DYNARRAY_NULLIFY(thread_runing_lists[i]);
        DYNARRAY_NULLIFY(thread_waiting_lists[i]);
        DYNARRAY_NULLIFY(thread_destroy_lists[i]);

        DYNARRAY_NULLIFY(thread_privilege_lists[i]);
    }

    //	DYNARRAY_NULLIFY(process_created_queue);

    for(int i = 0; i < MAX_PROCESS_SUPPORT; i++)
    {
        DYNARRAY_NULLIFY(process_lookup_lists[i]);
    }
}


size_t process_lookuplist_push(PROCESS_t* proc)
{
	uint16_t listPosition = (arch_pte_get_entry(&proc->arch.mmu)&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

	//	size_t idx = DYNARRAY_PUSHBACK(process_created_queue, proc);

	size_t idx = DYNARRAY_PUSHBACK(process_lookup_lists[listPosition], proc);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    KDBG("The position for the %d process_lookup_lists:%d \n", listPosition, process_lookup_lists[listPosition].length);
    return idx;
}

PROCESS_t* getProcessWithPid(pid_t pidTmp){
	if(process_vector.length>pidTmp){ 	//	if(process_created_queue.length>pidTmp){
		return process_vector.storage[pidTmp];	//	process_created_queue.storage[pidTmp];
	}
	return NULL;
}

PROCESS_t* lookupProcessWithPageTableAddress(size_t pageTableAddrToCheck){
	uint16_t listPosition = (pageTableAddrToCheck&0xFFFF0000)>>16;
	if(process_lookup_lists[listPosition].length == 0) return NULL;
	if(process_lookup_lists[listPosition].length == 1) return process_lookup_lists[listPosition].storage[0];
	if(process_lookup_lists[listPosition].length > 1){
		for(int i=0; i<process_lookup_lists[listPosition].length; i++){
			if(arch_pte_get_entry(&process_lookup_lists[listPosition].storage[i]->arch.mmu)==pageTableAddrToCheck){
				return process_lookup_lists[listPosition].storage[i];
			}
		}
	}

	return NULL;	//	This should never reached
}

void process_lookuplist_remove(PROCESS_t* proc)
{
	uint16_t listPosition = (arch_pte_get_entry(&proc->arch.mmu)&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

	proc->__state = PROCESS_STATE_EXIT;		//	The process would not be removed from process_created_queue

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(process_lookup_lists[listPosition], proc);
}

size_t thread_fifo_runqueue_push(THREAD_t* thread)
{
    uint8_t prio = thread->priority;
    size_t idx = DYNARRAY_PUSHBACK(thread_fifo_running_lists[prio], thread);
    return idx;
}

size_t thread_fifo_runqueue_add_first(THREAD_t* thread)
{
    uint8_t prio = thread->priority;	//	10;	//
    //ASSERT(prio < MAX_PRIORITYS);
    size_t idx = DYNARRAY_ADD_FIRST_ITEM(thread_fifo_running_lists[prio], thread);
    return idx;
}

THREAD_t* thread_fifo_runqueue_pop(void)
{
	THREAD_t* ret = NULL;

    int i = MAX_PRIORITYS-1;	//	0;

    for( ; i >= 0; i--)	//	for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_fifo_running_lists[i].length)
        {
            ret = thread_fifo_running_lists[i].storage[0];

            DYNARRAY_REMOVE_AND_PACK(thread_fifo_running_lists[i], 0);
            break;
        }
    }

    return ret;
}

void thread_fifo_runqueue_remove(THREAD_t* thread)
{
    uint8_t prio = thread->priority;
    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_fifo_running_lists[prio], thread);
}



size_t thread_fifo_waitqueue_push(THREAD_t* thread)
{
    uint8_t prio = thread->priority;
    size_t idx = DYNARRAY_PUSHBACK(thread_fifo_waiting_lists[prio], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //KDBG("The size of the %d thread_waiting_lists:%d \n", prio, thread_waiting_lists[prio].length);
    return idx;
}
//THREAD_t* thread_fifo_waitqueue_pop(void)
//{
//	THREAD_t* ret = NULL;
//    size_t i = 0;
//
//    for( ; i < MAX_PRIORITYS; i++)
//    {
//        if(thread_fifo_waiting_lists[i].length)
//        {
//            ret = thread_fifo_waiting_lists[i].storage[0];
//            DYNARRAY_REMOVE_AND_PACK(thread_fifo_waiting_lists[i], 0);
//            break;
//        }
//    }
//
//    return ret;
//}
THREAD_t* thread_fifo_waitqueue_get(int iThreadID)
{
	THREAD_t* ret = NULL;
    int i = 0;

    for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_fifo_waiting_lists[i].length>iThreadID)
        {
            ret = thread_fifo_waiting_lists[i].storage[iThreadID];
            break;
        }
    }

    return ret;
}
void thread_fifo_waitqueue_remove(THREAD_t* thread)
{
    uint8_t prio = thread->priority;
    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_fifo_waiting_lists[prio], thread);
}



size_t thread_runqueue_push(THREAD_t* thread)
{
	//	Todo: Here may be bugs that the priority changed from 10 to 1 here without related code found
    uint8_t prio = thread->priority;	//	10;	//

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(thread_runing_lists[prio], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //	DBG("The size of the %d thread_runing_lists:%d \n", prio, thread_runing_lists[prio].length);
    return idx;
}

size_t thread_runqueue_add_first(THREAD_t* thread)
{
    uint8_t prio = thread->priority;	//	10;	//

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_ADD_FIRST_ITEM(thread_runing_lists[prio], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //	DBG("The size of the %d thread_runing_lists:%d \n", prio, thread_runing_lists[prio].length);
    return idx;
}

THREAD_t* thread_runqueue_pop(void)
{
	THREAD_t* ret = NULL;
    int i = MAX_PRIORITYS-1;	//	0;

    for( ; i >= 0; i--)	//	for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_runing_lists[i].length)
        {
            ret = thread_runing_lists[i].storage[0];

            DYNARRAY_REMOVE_AND_PACK(thread_runing_lists[i], 0);
            break;
        }
    }
    //if(i == MAX_PRIORITYS) ret = NULL;
    return ret;
}

THREAD_t* getThreadNextToRun(void){
	THREAD_t* pThreadToRun = thread_fifo_runqueue_pop();
	if(pThreadToRun==NULL){
		pThreadToRun = thread_privilegequeue_pop();
		if(pThreadToRun!=NULL){
			pThreadToRun->priority = 255;
			pThreadToRun->schedulePolicy = SCHEDULE_POLICY_NORMAL;
		}
	}
	if(pThreadToRun==NULL){
		pThreadToRun = thread_runqueue_pop();
	}
	return pThreadToRun;
}

void thread_runqueue_remove(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    //ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_runing_lists[prio], thread);
}

/////////////////////////////////////////////////////////////////////////


size_t thread_privilegequeue_push(int iPrLevel, THREAD_t* thread)
{
    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(thread_privilege_lists[iPrLevel], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //KDBG("The size of the %d thread_privilege_lists:%d \n", iPrLevel, thread_privilege_lists[iPrLevel].length);
    return idx;
}


THREAD_t* thread_privilegequeue_pop(void)
{
	THREAD_t* ret = NULL;

    int i = MAX_PRIORITYS-1;	//	0;

    for( ; i >= 0; i--)	//	for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_privilege_lists[i].length)
        {
            ret = thread_privilege_lists[i].storage[0];

            DYNARRAY_REMOVE_AND_PACK(thread_privilege_lists[i], 0);
            break;
        }
    }


    return ret;
}

//	void thread_privilegequeue_remove(THREAD_t* thread)
//	{
//    	uint8_t prio = thread->priority;
//    	//ASSERT(prio < MAX_PRIORITYS);
//    	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_privilege_lists[prio], thread);
//	}

//////////////////////////////////////////////////////////////////////////////////////////
//	THREAD* thread_waitqueue_get(int iThreadID){return NULL;}

size_t thread_waitqueue_push(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(thread_waiting_lists[prio], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    KDBG("The size of the %d thread_waiting_lists:%d \n", prio, thread_waiting_lists[prio].length);
    return idx;
}


THREAD_t* thread_waitqueue_pop(void)
{
	THREAD_t* ret = NULL;
    size_t i = 0;

    for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_waiting_lists[i].length)
        {
            ret = thread_waiting_lists[i].storage[0];

            DYNARRAY_REMOVE_AND_PACK(thread_waiting_lists[i], 0);
            break;
        }
    }
    //if(i == MAX_PRIORITYS) ret = NULL;

    return ret;
}

THREAD_t* thread_waitqueue_get(int iThreadID)
{
	THREAD_t* ret = NULL;
    size_t i = 0;

    for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_waiting_lists[i].length>iThreadID)
        {
            ret = thread_waiting_lists[i].storage[iThreadID];

            //DYNARRAY_REMOVE_AND_PACK(thread_waiting_lists[i], 0);
            break;
        }
    }

    return ret;
}

void thread_waitqueue_remove(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    //ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_waiting_lists[prio], thread);
}

size_t thread_destroyqueue_push(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(thread_destroy_lists[prio], thread);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    KDBG("The size of the %d thread_waiting_lists:%d \n", prio, thread_destroy_lists[prio].length);
    return idx;
}

#ifdef RT_USING_SMP
void schedule_remove_thread(THREAD_t* thread)
{
   //TODO
}
#else
void schedule_remove_thread(THREAD_t* thread)
{
    //	phy_base_t level;

    ASSERT(thread != NULL);

    /* disable interrupt */
    //irq_disable();

    DBG("remove thread[%s], the priority: %d\n", thread->name, thread->priority );

    /* remove thread from ready list */
    thread_waitqueue_remove(thread);

    /* enable interrupt */
   // irq_enable();
}
#endif /* RT_USING_SMP */



/*
DYNARRAY_STRUCT(THREAD_t *) thread_runing_lists[MAX_PRIORITYS];

void scheduler_init()
{
    for(uint8_t i = 0; i < MAX_PRIORITYS; i++)
    {
        DYNARRAY_NULLIFY(thread_runing_lists[i]);
    }
}

size_t thread_runqueue_push(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_INSERT(thread_runing_lists[prio], thread);

    return idx;
}


THREAD_t* thread_runqueue_pop(void)
{
    THREAD_t* ret = NULL;
    size_t i = 0;

    for( ; i < MAX_PRIORITYS; i++)
    {
        if(thread_runing_lists[i].length)
        {
            ret = thread_runing_lists[i].storage[0];

            DYNARRAY_REMOVE_AND_PACK(thread_runing_lists[i], 0);
            break;
        }
    }
    //if(i == MAX_PRIORITYS) ret = NULL;

    return ret;
}

void thread_runqueue_remove(THREAD_t* thread)
{
    uint8_t prio = thread->priority;

    ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(thread_runing_lists[prio], thread);
}
*/

static void update_sched_time(THREAD_t* prev, THREAD_t* next)
{
    if (prev && prev->process)
    {
        systime_t diff;

        sub_systime(NULL, &prev->process->run_start, &diff);
        add_systime(&prev->process->run_time, &diff, &prev->process->run_time);
    }
    get_systime(&next->process->run_start);
}

void start_sched(void)
{
    THREAD_t* prev = CURRENT_THREAD;
    THREAD_t* next = getThreadNextToRun();	//	thread_runqueue_pop();

    if(NULL == next)
    {
		DBG("Start to schedule idle thread... \n");
        next = IDLE_THREAD;
    }

    CURRENT_THREAD = next;

    update_sched_time(NULL, next);
    arch_task_switch(prev, next);
}


void start_sched_intr_privilege(THREAD_t* pPrivilegeThread, iframe_t * iframe)
{
	THREAD_t* prev = CURRENT_THREAD;
	THREAD_t* next = pPrivilegeThread;

    if(next->state == THREAD_STATE_READY){
    	next->state = THREAD_STATE_RUNNING;
    }

    if(NULL == next)
    {
	    // KDBG("Start to schedule idle thread from thread:%p proc:%p\n", prev, prev->process);
        next = IDLE_THREAD;
    }else{
	    // KDBG("Start to schedule thread:%p proc:%p from thread:%p proc:%p\n", next, next->process, prev, prev->process);
    }

    CURRENT_THREAD = next;
    if((prev!=NULL)&&(prev->process!=NULL)&&(prev->is_user_thread)){
    	saveRegsFrame(&(prev->regs), iframe);
/*		prev->arch.rsp 	= iframe->rsp; //	(prev->arch.rsp&0xFFFFFFFF00000000 |  (iframe->rsp&0xFFFFFF) );
		prev->regs.rdi	= iframe->rdi;		//	uint64_t
		prev->regs.rsi	= iframe->rsi;		//	uint64_t
		prev->regs.rdx	= iframe->rdx;		//	uint64_t
		prev->regs.r10	= iframe->r10;		//	uint64_t
		prev->regs.r8	= iframe->r8;		//	uint64_t
		prev->regs.r9	= iframe->r9;		//	uint64_t
		prev->regs.rax	= iframe->rax;		//	uint64_t
		prev->regs.rbx	= iframe->rbx;		//	uint64_t
		prev->regs.rbp	= iframe->rbp;		//	uint64_t
		prev->regs.rcx	= iframe->rcx;		//	uint64_t
		prev->regs.r11	= iframe->r11;		//	uint64_t
		prev->regs.r12	= iframe->r12;		//	uint64_t
		prev->regs.r13	= iframe->r13;		//	uint64_t
		prev->regs.r14	= iframe->r14;		//	uint64_t
		prev->regs.r15	= iframe->r15;		//	uint64_t
		prev->regs.rip	= iframe->rip;		//	uint64_t
		prev->regs.cs	= iframe->cs;		//	uint32_t
		prev->regs.rflags= iframe->rflags;		//	uint64_t
		prev->regs.rsp	= iframe->rsp;		//	uint64_t
		prev->regs.ss	= iframe->ss;		//	uint32_t
*/
		if(prev->state==THREAD_STATE_EXIT){
			thread_destroyqueue_push(next);	//	thread_waitqueue_push(prev);
		}else{
			if(prev->schedulePolicy==SCHEDULE_POLICY_FIFO){
				thread_fifo_runqueue_add_first(prev);
			}else{
				thread_runqueue_push(prev);
			}

		}
	}

    if((next->schedulePolicy==SCHEDULE_POLICY_NORMAL)&&(next->priority>THREAD_PRIORITY_LEVEL_NORMAL)){
    	next->priority--;
    }

    update_sched_time(prev, next);
    arch_task_switch_intr(prev, next, iframe);
    return;
}


bool start_sched_intr(iframe_t * iframe, int fWithNormalQueue, THREAD_t* pThreadNextToRun)
{
	THREAD_t* 	prev = CURRENT_THREAD;

	int			fThreadStatusCheckInNeed = 1;

	THREAD_t* 	next = NULL;

	if(pThreadNextToRun == NULL){

		next = getThreadNextToRun();	//	thread_runqueue_pop();

		if(NULL == next)
	    {
	    	WARN("Can't schedule to idle thread from thread:%p proc:%p-%d\n", prev, prev->process, prev->process->pid);
			return false;
	    }else{
	    	// KDBG("Start to schedule thread:%p proc:%p from thread:%p proc:%p\n", next, next->process, prev, prev->process);
	    }

	    //if(strcmp( next->process->name, "slicing.elf")==0){
	    //	DBG("Found debug application.\n");
	    //}

	    //???	if or while need modified here
		//	2022-08 Assume the service/proc would always be with status ready or running
		while( 1 == fThreadStatusCheckInNeed ){
			if(next->state == THREAD_STATE_SIGWAITINFO){
				if(next->schedulePolicy==SCHEDULE_POLICY_FIFO){
					thread_fifo_runqueue_push(next);
				}else{
					thread_runqueue_push(next);
				}//thread_runqueue_push(next);
				next = getThreadNextToRun();	//	thread_runqueue_pop();
			}else if(next->state == THREAD_STATE_SIGWAIT_INTREVENT){
				if(next->schedulePolicy==SCHEDULE_POLICY_FIFO){
					thread_fifo_runqueue_push(next);
				}else{
					thread_runqueue_push(next);
				}//thread_runqueue_push(next);
				next = getThreadNextToRun();	//	thread_runqueue_pop();
			}
			//else if(next->state == THREAD_STATE_SIGWAIT_YIELD){
			//	thread_runqueue_push(next);
			//	next = thread_runqueue_pop();
			//}
			else if(next->state == THREAD_STATE_NANOSLEEP){
				//	DBG("Bypass thread for SLEEP ... \n");
				if(next->schedulePolicy==SCHEDULE_POLICY_FIFO){
					thread_fifo_runqueue_push(next);
				}else{
					thread_runqueue_push(next);
				}//thread_runqueue_push(next);
				next = getThreadNextToRun();	//	thread_runqueue_pop();
			}else if(next->state == THREAD_STATE_EXIT){
				thread_destroyqueue_push(next);	//	thread_waitqueue_push(next);	//	The exit thread not push back to running threads queues
				next = getThreadNextToRun();	//	thread_runqueue_pop();
			}else{
				fThreadStatusCheckInNeed = 0;
			}
		}

		//	For checking the re-scheduled threads after sleep or wait interrupt
	    if(next->state == THREAD_STATE_READY){
	    	next->state = THREAD_STATE_RUNNING;
	    }
	}else{
		next = pThreadNextToRun;
	}

    //	KDBG("\nStart to schedule thread:%p process:%p(%u:%s) ...\n", next, next->process, next->process->pid, next->process->name);

    CURRENT_THREAD = next;

    if((prev!=NULL)&&(prev->process!=NULL)&&(prev->is_user_thread)){
    	saveRegsFrame(&(prev->regs), iframe);
/*		prev->arch.rsp 	= iframe->rsp; //	(prev->arch.rsp&0xFFFFFFFF00000000 |  (iframe->rsp&0xFFFFFF) );
		prev->regs.rdi	= iframe->rdi;		//	uint64_t
		prev->regs.rsi	= iframe->rsi;		//	uint64_t
		prev->regs.rdx	= iframe->rdx;		//	uint64_t
		prev->regs.r10	= iframe->r10;		//	uint64_t
		prev->regs.r8	= iframe->r8;		//	uint64_t
		prev->regs.r9	= iframe->r9;		//	uint64_t
		prev->regs.rax	= iframe->rax;		//	uint64_t
		prev->regs.rbx	= iframe->rbx;		//	uint64_t
		prev->regs.rbp	= iframe->rbp;		//	uint64_t
		prev->regs.rcx	= iframe->rcx;		//	uint64_t
		prev->regs.r11	= iframe->r11;		//	uint64_t
		prev->regs.r12	= iframe->r12;		//	uint64_t
		prev->regs.r13	= iframe->r13;		//	uint64_t
		prev->regs.r14	= iframe->r14;		//	uint64_t
		prev->regs.r15	= iframe->r15;		//	uint64_t
		prev->regs.rip	= iframe->rip;		//	uint64_t
		prev->regs.cs	= iframe->cs;		//	uint32_t
		prev->regs.rflags= iframe->rflags;		//	uint64_t
		prev->regs.rsp	= iframe->rsp;		//	uint64_t
		prev->regs.ss	= iframe->ss;		//	uint32_t
*/
		if(fWithNormalQueue){
			if(prev->state==THREAD_STATE_EXIT){
				thread_destroyqueue_push(next);	//	thread_waitqueue_push(prev);
			}else{
				thread_runqueue_push(prev);
			}
		}

	}

    if((next->schedulePolicy==SCHEDULE_POLICY_NORMAL)&&(next->priority>THREAD_PRIORITY_LEVEL_NORMAL)){
    	next->priority--;
    }

    update_sched_time(prev, next);
    arch_task_switch_intr(prev, next, iframe);
	return true;
}
