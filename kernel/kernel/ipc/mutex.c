#include <types.h>
#include <mutex.h>
#include <utils/include/print_macros.h>
#include <semaphore.h>
#include <return_code.h>

#include "mm.h"
#include "arch.h"
#include "task.h"
#include "ipc.h"
#include "errno.h"
#include "kobj.h"

void phy_object_init(struct phy_object         *object,
                    enum phy_object_class_type type,
                    const char                 *name);

int _ipc_object_init(struct phy_ipc_object *ipc);

phy_object_t phy_object_allocate(enum phy_object_class_type type, const char *name);

void phy_object_detach(phy_object_t object);


void ipc_copy_in(THREAD_t* p_thread, vaddr_t src, paddr_t dst, size_t size);

 phy_base_t phy_hw_interrupt_disable();
 void phy_hw_interrupt_enable(phy_base_t level);


 err_t _ipc_list_resume_all(list_t *list);

err_t _ipc_list_suspend( list_t*      list,
                                THREAD_t*    thread,
                                uint8_t      flag);

err_t phy_thread_control(THREAD_t* thread, int cmd, void *arg);        


err_t phy_mutex_init(phy_mutex_t mutex, const char *name, uint8_t flag)
{
    (void)flag;

    /* parameter check */
    ASSERT(mutex != NULL);

    /* initialize object */
    phy_object_init(&(mutex->parent.parent), phy_Object_Class_Mutex, name);

    /* initialize ipc object */
    _ipc_object_init(&(mutex->parent));

    mutex->value = 1;
    mutex->owner = NULL;
    mutex->original_priority = 0xFF;
    mutex->hold  = 0;

    mutex->attr.protocol    = PHY_MUX_PRIO_INHERIT;	//协议默认用继承方式, A(4)task等B(19)释放锁时,B的调度优先级直接升到(4)
    mutex->attr.prioceiling = OS_TASK_PRIORITY_LOWEST;//最低优先级
    mutex->attr.type        = PHY_MUX_DEFAULT;		//默认 PHY_MUX_RECURSIVE

    /* flag can only be PHY_IPC_FLAG_PRIO.
         PHY_IPC_FLAG_FIFO cannot solve the unbounded priority inversion problem */
    mutex->parent.parent.flag = PHY_IPC_FLAG_PRIO;

    return EOK;
}

phy_mutex_t phy_mutex_create(const char *name, uint8_t flag)
{
    struct phy_mutex *mutex;

    //THREAD_t*  thread= CURRENT_THREAD;
   
    /* allocate object */
    mutex = (phy_mutex_t)phy_object_allocate(phy_Object_Class_Mutex, name);
    if (mutex == NULL)  return mutex;

    /* initialize ipc object */
    _ipc_object_init(&(mutex->parent));

    mutex->value              = 1;
    mutex->owner              = NULL;
    mutex->original_priority  = 0xFF;
    mutex->hold               = 0;

    /* flag can only be phy_IPC_FLAG_PRIO.
         phy_IPC_FLAG_FIFO cannot solve the unbounded priority inversion problem */
    mutex->parent.parent.flag = PHY_IPC_FLAG_PRIO;

    return mutex;
}

err_t phy_mutex_detach(phy_mutex_t mutex)
{
    /* parameter check */
    ASSERT(mutex != NULL);
    ASSERT(phy_object_get_type(&mutex->parent.parent) == phy_Object_Class_Mutex);
    ASSERT(phy_object_is_systemobject(&mutex->parent.parent));

    /* wakeup all suspended threads */
    _ipc_list_resume_all(&(mutex->parent.suspend_thread));

    /* detach mutex object */
    phy_object_detach(&(mutex->parent.parent));

    return EOK;
}

err_t phy_mutex_delete(phy_mutex_t mutex)
{

    /* parameter check */
    ASSERT(mutex != NULL);
    ASSERT(phy_object_get_type(&mutex->parent.parent) == phy_Object_Class_Mutex);
    ASSERT(phy_object_is_systemobject(&mutex->parent.parent) == false);

    /* wakeup all suspended threads */
    _ipc_list_resume_all(&(mutex->parent.suspend_thread));

    /* delete mutex object */
    phy_object_delete(&(mutex->parent.parent));

    return EOK;
}

err_t phy_thread_control(THREAD_t* thread, int cmd, void *arg)
{
    register phy_base_t temp;

    switch (cmd)
    {
        case PHY_THREAD_CTRL_CHANGE_PRIORITY:
        {
            /* disable interrupt */
           temp = phy_hw_interrupt_disable();

            /* for ready thread, change queue */
            if ((thread->state & THREAD_STATE_MASK) == THREAD_STATE_READY)
            {
                /* remove thread from schedule queue first */
               // phy_schedule_remove_thread(thread);

                /* change thread priority */
                thread->priority = *(uint8_t *)arg;


                /* insert thread to schedule queue again */
               // phy_schedule_insert_thread(thread);
            }

            phy_hw_interrupt_enable(temp);
        }
    }

    return EOK;

}

//	static
err_t phy_mutex_take(phy_mutex_t mutex, uint32_t time)
{
    register phy_base_t temp;

    THREAD_t* thread= CURRENT_THREAD;

    /* this function must not be used in interrupt even if time = 0 */

    /* parameter check */
    ASSERT(mutex != NULL);
    ASSERT(phy_object_get_type(&mutex->parent.parent) == phy_Object_Class_Mutex);

    /* get current thread */
  //  thread = phy_thread_self();

    /* disable interrupt */
    temp = phy_hw_interrupt_disable();  

    if (mutex->owner == thread)
    {
        if(mutex->hold < PHY_MUTEX_HOLD_MAX)
        {
            /* it's the same thread */
            mutex->hold ++;
        } else {
            phy_hw_interrupt_enable(temp); /* enable interrupt */
            return -EOVERFLOW; /* value overflowed */
        }
    }else{
        if (mutex->value > 0)
        {  /* mutex is available */
            mutex->value --;

             /* set mutex owner and original priority */
            mutex->owner             = thread;
            mutex->original_priority = thread->priority;
            if(mutex->hold < PHY_MUTEX_HOLD_MAX)
            {
                mutex->hold ++;
            } else {
                phy_hw_interrupt_enable(temp); /* enable interrupt */
                return -EOVERFLOW; /* value overflowed */
            }
        } else {
            /* no waiting, return with timeout */
            if (time == 0)
            {
                /* set error as timeout */
                thread->error = -ETIMEOUT;

                /* enable interrupt */
                phy_hw_interrupt_enable(temp);

                return -ETIMEOUT;
            } else {
                       /* change the owner thread priority of mutex */
                if (thread->priority > mutex->owner->priority)
                {
                    /* change the owner thread priority */
                    phy_thread_control(mutex->owner,
                                      PHY_THREAD_CTRL_CHANGE_PRIORITY,
                                      &thread->priority);
                }

                /* suspend current thread */
                _ipc_list_suspend(&(mutex->parent.suspend_thread),
                                    thread,
                                    mutex->parent.parent.flag);

                  /* has waiting time, start thread timer */
                if (time > 0)
                {                   
                    //phy_timer_start(&(thread->thread_timer));
                     addTimer(&(thread->thread_timer));
                }
            }
        }

    }

    /* enable interrupt */
    phy_hw_interrupt_enable(temp);
    
    /* do schedule */
    //	start_sched_intr();//  start_sched();
 
    return EOK;
}

err_t phy_mutex_trytake(phy_mutex_t mutex)
{
    return phy_mutex_take(mutex, PHY_WAITING_NO);
}

err_t phy_mutex_release(phy_mutex_t mutex)
{
    register phy_base_t temp;
     bool need_schedule;

      THREAD_t* thread= CURRENT_THREAD;

    /* parameter check */
    ASSERT(mutex != NULL);
    ASSERT(phy_object_get_type(&mutex->parent.parent) == phy_Object_Class_Mutex);    
    /* disable interrupt */
    temp = phy_hw_interrupt_disable();

      /* mutex only can be released by owner */
    if (thread != mutex->owner)
    {
        thread->error = -ERROR;

        /* enable interrupt */
        phy_hw_interrupt_enable(temp);

        return -ERROR;
    }

     /* decrease hold */
    mutex->hold --;

    /* if no hold */
    if (mutex->hold == 0)
    {
        /* change the owner thread to original priority */
        if (mutex->original_priority != mutex->owner->priority)
        {
            phy_thread_control(mutex->owner,
                              PHY_THREAD_CTRL_CHANGE_PRIORITY,
                              &(mutex->original_priority));
        }
    }

     /* wakeup suspended thread */
        if (!list_is_empty(&mutex->parent.suspend_thread))
        {
            /* get suspended thread */
            thread =(THREAD_t*) listEntry(mutex->parent.suspend_thread.next,
                                   THREAD_t,   thdList);

            printf("mutex_release: resume thread: %s\n", thread->name);

            /* set new owner and priority */
            mutex->owner             = thread;
            mutex->original_priority = thread->priority;

            if(mutex->hold < PHY_MUTEX_HOLD_MAX)
            {
                mutex->hold ++;
            } else  {
                phy_hw_interrupt_enable(temp); /* enable interrupt */
                return -EOVERFLOW; /* value overflowed */
            }

            /* resume thread */
            _ipc_list_resume(&(mutex->parent.suspend_thread));

            need_schedule = false;
        }  else  {
            if(mutex->value < PHY_MUTEX_VALUE_MAX)
            {
                /* increase value */
                mutex->value ++;
            } else  {
                phy_hw_interrupt_enable(temp); /* enable interrupt */
                return -EOVERFLOW; /* value overflowed */
            }

            /* clear owner */
            mutex->owner             = NULL;
            mutex->original_priority = 0xff;
        }

    /* enable interrupt */
    phy_hw_interrupt_enable(temp);

     /* perform a schedule */
    if (need_schedule == true)     start_sched();


    return EOK;
}

