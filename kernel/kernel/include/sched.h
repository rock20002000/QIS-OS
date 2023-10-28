#pragma once

#include <platform/include/kirk_task.h>
#include <task.h>
#include <utils/include/dyn_array.h>
#include "object.h"

extern DYNARRAY_STRUCT(THREAD_t *) thread_runing_lists[];		//	The normal running threads
extern DYNARRAY_STRUCT(THREAD_t *) thread_ready_lists[];
extern DYNARRAY_STRUCT(THREAD_t *) thread_waiting_lists[];		//	The threads just created waiting to be scheduled to run
extern DYNARRAY_STRUCT(THREAD_t *) thread_privilege_lists[];	//	The privileged threads to be run


/*
 * Per process flags
 */
#define PF_VCPU			0x00000001	/* I'm a virtual CPU */
#define PF_IDLE			0x00000002	/* I am an IDLE thread */
#define PF_EXITING		0x00000004	/* Getting shut down */
#define PF_POSTCOREDUMP		0x00000008	/* Coredumps should ignore this task */
#define PF_IO_WORKER		0x00000010	/* Task is an IO worker */
#define PF_WQ_WORKER		0x00000020	/* I'm a workqueue worker */
#define PF_FORKNOEXEC		0x00000040	/* Forked but didn't exec */
#define PF_MCE_PROCESS		0x00000080      /* Process policy on mce errors */
#define PF_SUPERPRIV		0x00000100	/* Used super-user privileges */
#define PF_DUMPCORE		0x00000200	/* Dumped core */
#define PF_SIGNALED		0x00000400	/* Killed by a signal */
#define PF_MEMALLOC		0x00000800	/* Allocating memory */
#define PF_NPROC_EXCEEDED	0x00001000	/* set_user() noticed that RLIMIT_NPROC was exceeded */
#define PF_USED_MATH		0x00002000	/* If unset the fpu must be initialized before use */
#define PF_NOFREEZE		0x00008000	/* This thread should not be frozen */
#define PF_FROZEN		0x00010000	/* Frozen for system suspend */
#define PF_KSWAPD		0x00020000	/* I am kswapd */
#define PF_MEMALLOC_NOFS	0x00040000	/* All allocation requests will inherit GFP_NOFS */
#define PF_MEMALLOC_NOIO	0x00080000	/* All allocation requests will inherit GFP_NOIO */
#define PF_LOCAL_THROTTLE	0x00100000	/* Throttle writes only against the bdi I write to,
/* I am cleaning dirty pages from some other bdi. */
#define PF_KTHREAD		0x00200000	/* I am a kernel thread */
#define PF_RANDOMIZE		0x00400000	/* Randomize virtual address space */
#define PF_SWAPWRITE		0x00800000	/* Allowed to write to swap */
#define PF_NO_SETAFFINITY	0x04000000	/* Userland is not allowed to meddle with cpus_mask */
#define PF_MCE_EARLY		0x08000000      /* Early kill for mce process policy */
#define PF_MEMALLOC_PIN		0x10000000	/* Allocation context constrained to zones which allow long term pinning. */
#define PF_FREEZER_SKIP		0x40000000	/* Freezer should not count it as freezable */
#define PF_SUSPEND_TASK		0x80000000      /* This thread called freeze_processes() and should not be frozen */

static size_t scheduler_lock_nest;


#ifdef USING_SMP
static inline void phy_enter_critical(void)
{
    //TODO
}
#else
static inline void phy_enter_critical(void)
{
    /* disable interrupt */
   // irq_disable();
    DBG("%s\n",__func__);
    /*
     * the maximal number of nest is UINTMAX,
     * which is big enough and does not check here
     */
    scheduler_lock_nest ++;

    /* enable interrupt */
 //  irq_enable();
    DBG("%s\n",__func__);
}
#endif /* USING_SMP */


#ifdef RT_USING_SMP
static inline void phy_exit_critical(void)
{
    //TODO
}
#else
static inline void phy_exit_critical(void)
{
    //	phy_base_t level;
    bool need_schedule = false;

    /* disable interrupt */
   // irq_disable();

    scheduler_lock_nest --;
    if (scheduler_lock_nest <= 0)
    {
        scheduler_lock_nest = 0;
        /* enable interrupt */
      //  irq_enable();
   
        /* if scheduler is started, do a schedule */
        if (need_schedule == true)  start_sched();
  
    }
    else
    {
        /* enable interrupt */
      //  irq_enable();
    }
}
#endif /* USING_SMP */

//#define PHY_NAME_MAX 32
#define PHY_ALIGN_SIZE 4
#define PHY_THREAD_PRIORITY_32
#define PHY_THREAD_PRIORITY_MAX 32


#ifdef USING_SMP
static inline void phy_schedule_insert_thread(THREAD_t* thread)
{
   //Todo
}
#else
static inline void phy_schedule_insert_thread(THREAD_t* thread)
{
    //	phy_base_t level;
    uint8_t prio = thread->priority;

    ASSERT(thread != NULL);

    /* disable interrupt */
   // irq_disable();

    /* it's current thread, it should be RUNNING thread */
    if (thread == CURRENT_THREAD)
    {
        thread->STATE_BITMAP = THREAD_STATE_RUNNING | (thread->state & ~THREAD_STATE_MASK);
        goto __exit;
    }

    /* READY thread, insert to ready queue */
    thread->STATE_BITMAP = THREAD_STATE_YIELD_MASK | (thread->state & ~THREAD_STATE_MASK);
    /* there is no time slices left(YIELD), inserting thread before ready list*/
    if((thread->state & THREAD_STATE_YIELD_MASK) != 0)
    {
       // list_insert_before(&(phy_thread_priority_table[thread->priority]),
           //                   &(thread->thdList)  );
        DYNARRAY_INSERT(thread_runing_lists[prio], thread);
       
    }
    /* there are some time slices left, inserting thread after ready list to schedule it firstly at next time*/
    else
    {
       // list_insert_after(&(phy_thread_priority_table[thread->priority]),
        //                      &(thread->thdList)  );
        DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    }

    DBG("insert thread[%s], the priority: %d\n", thread->name, thread->priority );

__exit:
	;
/* enable interrupt */
  //  irq_enable();
    DBG("__exit\n"  );

}
#endif /* USING_SMP */
