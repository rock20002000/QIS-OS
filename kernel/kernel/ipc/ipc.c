
#include "semaphore.h"
#include "errno.h"
#include "utils/include/list.h"
#include "object.h"
#include "libc/include/string.h"
#include "mm.h"
#include "arch.h"
#include "task.h"
#include "ipc.h"
#include "return_code.h"


int _ipc_object_init(struct phy_ipc_object *ipc)
{
    /* initialize ipc object */
    list_init(&(ipc->suspend_thread));

    return EOK;
}

phy_object_t phy_object_allocate(enum phy_object_class_type type, const char *name)
{
    struct phy_object *object;
    //	size_t level;
    struct phy_object_information *information;

    /* get object information */
    information = phy_object_get_information(type);
    ASSERT(information != NULL);

    object = (struct phy_object *)kmallocz(information->object_size);
    if (object == NULL)
    {
        /* no memory can be allocated */
        return NULL;
    }

    /* clean memory data of object */
 //   memset(object, 0x0, information->object_size);

    /* initialize object's parameters */

    /* set object type */
    object->type = type;

    /* set object flag */
    object->flag = 0;

    /* copy name */
    strncpy(object->name, name, PHY_NAME_MAX);

  //  RT_OBJECT_HOOK_CALL(rt_object_attach_hook, (object));

    /* lock interrupt */
   // level = hw_interrupt_disable();
 //  irq_disable();
 //    level = phy_hw_interrupt_disable();
        /* insert object into information object list */
   list_insert_after(&(information->object_list), &(object->list));

    DBG("Object->type:%d release\n",phy_object_get_type(object));
    /* unlock interrupt */
 //  hw_interrupt_enable(level);
  //  irq_enable();
   //   phy_hw_interrupt_enable(level);

    /* return object */
    return object;
}

err_t phy_thread_suspend(THREAD_t* thread);

 err_t _ipc_list_suspend( list_t*      list,
                                THREAD_t*    thread,
                                uint8_t      flag)
{
    /* suspend thread */
    phy_thread_suspend(thread);

    switch (flag)
    {
    case PHY_IPC_FLAG_FIFO:
        list_insert_before(list, &(thread->thdList));
        break; /* RT_IPC_FLAG_FIFO */

    case PHY_IPC_FLAG_PRIO:
        {
            list_t *n;
            THREAD_t *sthread;

            /* find a suitable position */
            for (n = list->next; n != list; n = n->next)
            {
                sthread = containerOf(n, THREAD_t, thdList);

                /* find out */
                if (thread->priority < sthread->priority)
                {
                    /* insert this thread before the sthread */
                    list_insert_before(&(sthread->thdList), &(thread->thdList));
                    break;
                }
            }
            /*
             * not found a suitable position,
             * append to the end of suspend_thread list
             */
            if (n == list)
                list_insert_before(list, &(thread->thdList));
        }
        break;/* RT_IPC_FLAG_PRIO */

    default:
        ASSERT(0);
        break;
    }

    return EOK;
}



err_t phy_thread_resume(THREAD_t* thread)
{
    /* parameter check */
    ASSERT(thread != NULL);
    ASSERT(phy_object_get_type((phy_object_t)thread) == phy_Object_Class_Thread);

    DBG("thread resume:  %s\n", thread->name);

    if ((thread->state & THREAD_STATE_MASK ) != THREAD_STATE_BLOCKED)
    {
        DBG("thread resume: thread disorder, %d\n",  thread->state );

        return -ERROR;
    }

    /* disable interrupt */
 //   irq_disable();
// level =   phy_hw_interrupt_disable();

    /* remove from suspend list */
   // list_remove(&(thread->tlist));
   thread_waitqueue_remove(thread);

   // rt_timer_stop(&thread->thread_timer);

    /* insert to schedule ready list */
    //schedule_insert_thread(thread);
    thread_runqueue_remove(thread);

    /* enable interrupt */
   // irq_enable();
  // phy_hw_interrupt_enable(level);

    //RT_OBJECT_HOOK_CALL(rt_thread_resume_hook, (thread));
    return EOK;
}

 err_t _ipc_list_resume_all(list_t *list)
{
    THREAD_t *thread;

    /* wakeup all suspended threads */
    while (!list_is_empty(list))
    {
        /* disable interrupt */
         //   irq_disable();
   //     level = phy_hw_interrupt_disable();

        /* get next suspended thread */
     //   thread = containerOf(list->next, THREAD_t, thdList);
          thread =  thread_waitqueue_pop();
        /* set error code to RT_ERROR */
        thread->error = -ERROR;

        /*
         * resume thread
         * In thread_resume function, 
         * it will remove current thread from suspended list
         */
        phy_thread_resume(thread);

        /* enable interrupt */
      //  irq_enable();
     //  phy_hw_interrupt_enable(level);
    }

    return EOK;
}

int phy_sem_delete(phy_sem_t sem)
{
    /* parameter check */
    ASSERT(sem != NULL);
    ASSERT(phy_object_get_type(&sem->parent.parent) == phy_Object_Class_Semaphore);
    ASSERT(phy_object_is_systemobject(&sem->parent.parent) == false);

    /* wakeup all suspended threads */
    _ipc_list_resume_all(&(sem->parent.suspend_thread));

    /* delete semaphore object */
    phy_object_delete(&(sem->parent.parent));

    return EOK;
}


int phy_sem_take_notimer(phy_sem_t sem)
{
    //	THREAD_t* thread;

    /* parameter check */
    ASSERT(sem != NULL);
    ASSERT(phy_object_get_type(&sem->parent.parent) == phy_Object_Class_Semaphore);

    /* disable interrupt */
   // level = hw_interrupt_disable();
 //   level =   phy_hw_interrupt_disable();

    if (sem->value > 0)
    {
        /* semaphore is available */
        sem->value --;
          /* enable interrupt */
    //    irq_enable();
       //  phy_hw_interrupt_enable(level);
     //   DBG(" \n sem value:%d\n",sem->value);
    } else {
        /* no waiting, return with timeout */
    //     irq_enable();
    //     phy_hw_interrupt_enable(level);
     //    DBG(" \n sem value:%d\n",sem->value);
         return -ETIMEDOUT;     
    }
  //   DBG(" \n sem value:%d\n",sem->value);
    //RT_OBJECT_HOOK_CALL(rt_object_take_hook, (&(sem->parent.parent)));
    return sem->value;
}

err_t phy_sem_take(phy_sem_t sem, size_t timeout)
{
    	size_t level;
    THREAD_t* thread;

    /* parameter check */
    ASSERT(sem != NULL);
    ASSERT(phy_object_get_type(&sem->parent.parent) == phy_Object_Class_Semaphore);

  //  RT_OBJECT_HOOK_CALL(rt_object_trytake_hook, (&(sem->parent.parent)));

    /* disable interrupt */
    level = phy_hw_interrupt_disable();
  //  irq_disable();

   // RT_DEBUG_LOG(RT_DEBUG_IPC, ("thread %s take sem:%s, which value is: %d\n",
       ///                         rt_thread_self()->name,
        //                        ((struct rt_object *)sem)->name,
        ///                        sem->value));

    if (sem->value > 0)
    {
        /* semaphore is available */
        sem->value --;

        /* enable interrupt */
        phy_hw_interrupt_enable(level);
     //    irq_enable();
    }
    else
    {
        /* no waiting, return with timeout */
        if (timeout == 0)
        {
          //  irq_enable();
             phy_hw_interrupt_enable(level);
            return -ETIMEDOUT;
        }
        else
        {
            /* current context checking */
           // RT_DEBUG_SCHEDULER_AVAILABLE(RT_TRUE);

            /* semaphore is unavailable, push to suspend list */
            /* get current thread */
           // thread = rt_thread_self();
            thread= CURRENT_THREAD;

            /* reset thread error number */
            thread->error = EOK;

            DBG("sem take: suspend thread - %s\n", thread->name);

            /* suspend thread */
            _ipc_list_suspend(&(sem->parent.suspend_thread),
                                thread,
                                sem->parent.parent.flag);
           
            /* enable interrupt */
             phy_hw_interrupt_enable(level);

          //  irq_enable();
            /* do schedule */
           // rt_schedule();
        //   start_sched();

            if (thread->error != EOK)
            {
                return thread->error;
            }
        }
    }

    //RT_OBJECT_HOOK_CALL(rt_object_take_hook, (&(sem->parent.parent)));

    return EOK;
}



phy_sem_t phy_sem_create(const char *name, uint32_t value, uint8_t flag)
{
    phy_sem_t sem;
  
    /* allocate object */
    sem = (phy_sem_t)phy_object_allocate(phy_Object_Class_Semaphore, name);
    
    if (sem == NULL)   return sem;

    /* initialize ipc object */
    _ipc_object_init(&(sem->parent));

    /* set initial value */
    sem->value = value;

    /* set parent */
    sem->parent.parent.flag = flag;
 
    return sem;
}

extern void phy_object_init(struct phy_object *object,
                    enum phy_object_class_type type,
                    const char                 *name);
err_t phy_sem_init(phy_sem_t    sem,
                     const char *name,
                     uint32_t value,
                     uint8_t  flag)
{
    ASSERT(sem != NULL);
    ASSERT(value < 0x10000U);
    ASSERT((flag == PHY_IPC_FLAG_FIFO) || (flag == PHY_IPC_FLAG_PRIO));
    DBG("%s sem:0x%p name:%s\n",__func__, sem,name);
    /* initialize object */
    phy_object_init(&(sem->parent.parent), phy_Object_Class_Semaphore, name);
   
    /* initialize ipc object */
    _ipc_object_init(&(sem->parent));
    
    /* set initial value */
    sem->value = (uint16_t)value;
 
    /* set parent */
    sem->parent.parent.flag = flag;

    return EOK;
}

int sem_timedwait(sem_t *sem, const  size_t abs_timeout)
{
    err_t result;
    size_t tick;

    if (!sem || !abs_timeout)
        return EINVAL;

    /* calculate os tick */
    tick = getTicks();

    result = phy_sem_take(sem->sem, tick);
    if (result == -ETIMEOUT)
    {
     //   rt_set_errno(ETIMEDOUT);

        return -1;
    }
    if (result == EOK)   return 0;

//    rt_set_errno(EINTR);

    return -1;
}



