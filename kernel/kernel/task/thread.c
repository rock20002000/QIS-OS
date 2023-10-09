#include <libc/include/string.h>
#include <libc/include/stdio.h>

#include "task.h"
#include "arch_task.h"
#include "kernel.h"
#include <errno.h>

static id_t thread_id = 0;

THREAD_t* init_thread(THREAD_t* thread, const char* name, thread_func function, uint8_t priority, bool is_user) 
{
    bool alloc_thread = thread == NULL;

    if (alloc_thread) 
    {
        // Allocate one page as tcb_t and kernel stack for each thread.
        thread = (THREAD_t*)kmallocz(sizeof(THREAD_t));
    }
    if (!thread)
        return NULL;

    if (name != NULL) 
    {
        ASSERT(strlen(name) < CONFIG_THREAD_NAME_LEN);

        strncpy(thread->name, name, CONFIG_THREAD_NAME_LEN);
    } 
    else 
    {
        snprintf(thread->name, CONFIG_THREAD_NAME_LEN, "thread-%u", thread_id++);
    }
    thread->state 			= THREAD_STATE_BLOCKED;
    thread->priority 		= priority;
    thread->schedulePolicy	= SCHEDULE_POLICY_NORMAL;
    
    thread->thread_settings.fIOAccess = 0;

    thread->quantum 		= 0;
    thread->is_user_thread 	= is_user;

    arch_thread_init(thread, (uintptr_t)function);

    return thread;
}

THREAD_t* init_thread_withParam(THREAD_t* thread, const char* name, thread_func function,
    void* threadParams, void* extParam, THREAD_t* pCurThread, uint8_t priority, bool is_user)
{
    size_t thread_id = 0;
    bool alloc_thread = thread == NULL;

    if(pCurThread!=NULL){
    	thread_id = pCurThread->process->threads.length;
    }
    if (alloc_thread) 
    {
        // Allocate one page as tcb_t and kernel stack for each thread.
        thread = (THREAD_t*)kmallocz(sizeof(THREAD_t));
    }
    if (!thread)
        return NULL;

    if (name != NULL)
    {
        ASSERT(strlen(name) < CONFIG_THREAD_NAME_LEN);

        strncpy(thread->name, name, CONFIG_THREAD_NAME_LEN);
    }
    else
    {
        snprintf(thread->name, CONFIG_THREAD_NAME_LEN, "thread-%u", thread_id++);
    }
    //Step4: Alloc&map page for thread kernel stack
    thread->state 			= THREAD_STATE_BLOCKED;
    thread->priority 		= priority;
    thread->schedulePolicy	= SCHEDULE_POLICY_NORMAL;

    thread->thread_settings.fIOAccess = 0;

    thread->quantum 		= 0;
    thread->is_user_thread 	= is_user;

    arch_thread_init_withParam(thread, (uintptr_t)function, threadParams, extParam, pCurThread);

    return thread;
}

/*build up proc with pid 0*/
THREAD_t* msg_pthread_create(PROCESS_t* proc, pthread_t*  thread_id, void* threadAttrs, start_rtn handler, void* threadParams, int iStartStatus)
{
	THREAD_t*  thread = init_thread(NULL, proc->name, handler, THREAD_PRIORITY_LEVEL_NORMAL ,true);	//	10

    process_bind_thread(proc, thread);

    // KDBG("bind thread 0x%p  to process 0x%p pid:%d with entry:%p\n", thread, proc, proc->pid, handler);

    //	thread_runqueue_push(thread);

    if(iStartStatus==1){
    	thread->state = THREAD_STATE_READY;
    }else{
    	thread->state = THREAD_STATE_WAITING;	//THREAD_STATE_SIGWAIT_YIELD;
    }

    //if(iStartStatus==0){
    //	thread_waitqueue_push(thread);
    //}
    if(iStartStatus==1){
    	thread_runqueue_push(thread);
    }else{
    	if(thread->schedulePolicy==SCHEDULE_POLICY_FIFO){
    		thread_fifo_waitqueue_push(thread);
    	}else{
    		thread_waitqueue_push(thread);
    	}
    }

	return thread;//0;
}

THREAD_t* msg_pthread_create_withParam(THREAD_t * pCurThread, pthread_t*  thread_id, void* threadAttrs, start_rtn_withParam handler, void* threadParams, void* extParam, int iStartStatus){

	PROCESS_t* prpProcTmp = pCurThread->process;	//	lookupProcessWithPageTableAddress(pageTableAddress);
	THREAD_t*  threadTmp = init_thread_withParam(NULL, NULL, handler, threadParams, extParam, pCurThread, THREAD_PRIORITY_LEVEL_NORMAL ,true);	//	10


	//	printf("prpProcTmp thread count:%d\n", prpProcTmp->threads.length );

    process_bind_thread(prpProcTmp, threadTmp);

    //	printf("prpProcTmp thread tid:%d\n", threadTmp->tid );

    copy_kernel_data(NULL, (thread_id), sizeof(size_t),  &(threadTmp->tid));

    // KDBG("bind thread 0x%p  to process 0x%p pid:%d with entry:%p\n", threadTmp, prpProcTmp, prpProcTmp->pid, handler);

    //	thread_runqueue_push(threadTmp);

    if(iStartStatus==1){
    	threadTmp->state = THREAD_STATE_READY;
    }else{
    	threadTmp->state = THREAD_STATE_WAITING;	//THREAD_STATE_SIGWAIT_YIELD;
    }

    //if(iStartStatus==0){
    //	thread_waitqueue_push(threadTmp);
    //}
    if(iStartStatus==1){
    	thread_runqueue_push(threadTmp);
    }else{
    	if(threadTmp->schedulePolicy==SCHEDULE_POLICY_FIFO){
    		thread_fifo_waitqueue_push(threadTmp);
    	}else{
    		thread_waitqueue_push(threadTmp);
    	}
    }

	return NULL;//threadTmp;//0;
}

size_t thread_stack_size(THREAD_t* thread)
{
    return THREAD_STACK_SIZE;
}

void schedule_remove_thread(THREAD_t* thread);
err_t phy_thread_suspend(THREAD_t* thread)
{
    phy_base_t stat;
    //	phy_base_t level;

    /* parameter check */
    ASSERT(thread != NULL);
    ASSERT(phy_object_get_type((phy_object_t)thread) == phy_Object_Class_Thread);
    ASSERT(thread == CURRENT_THREAD);

    DBG("thread suspend:  %s\n", thread->name);

    stat = thread->state & THREAD_STATE_MASK;
    if ((stat != THREAD_STATE_READY) && (stat != THREAD_STATE_RUNNING))
    {
        DBG("thread suspend: thread disorder, 0x%2x\n", thread->state);
        return -ERROR;
    }

    /* disable interrupt */
   // irq_disable();

    /* change thread stat */
    schedule_remove_thread(thread);
    thread->state = THREAD_STATE_BLOCKED | (thread->state & ~THREAD_STATE_MASK);

    /* stop thread timer anyway */
   //timer_stop(&(thread->thread_timer));

    /* enable interrupt */
 //   irq_enable();

    //RT_OBJECT_HOOK_CALL(rt_thread_suspend_hook, (thread));
    return EOK;
}

int check_thread_privilege(THREAD_t* thread, THREAD_SETTING_INDEX privilegeToCheck){
	switch(privilegeToCheck){
	case THREAD_CTL_IO:
		return 1;	//	return thread->thread_settings.fIOAccess;
		break;
	default:
		return 1;
	}
	return 0;
}

void destroy_thread(THREAD_t* thread, bool deattach)
{
    schedule_remove_thread(thread);
    arch_thread_destroy(thread);
    if (deattach)
        process_unbind_thread(thread->process, thread);
    kfree(thread);
}
