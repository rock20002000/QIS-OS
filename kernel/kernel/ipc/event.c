
#include "event.h"
#include <return_code.h>
#include "errno.h"
#include "object.h"
#include "task.h"
#include "kobj.h"
#include "ipc.h"

err_t phy_event_init(phy_event_t event, const char *name, uint8_t flag)
{
    /* parameter check */
    ASSERT(event != NULL);

    /* initialize object */
    phy_object_init(&(event->parent.parent), phy_Object_Class_Event, name);

    /* set parent flag */
    event->parent.parent.flag = flag;

    /* initialize ipc object */
    _ipc_object_init(&(event->parent));

    /* initialize event */
    event->eventSet = 0;

    return EOK;
}

err_t phy_event_detach(phy_event_t event)
{
    /* parameter check */
    ASSERT(event != NULL);
    ASSERT(phy_object_get_type(&event->parent.parent) == phy_Object_Class_Event);
    ASSERT(phy_object_is_systemobject(&event->parent.parent));

    /* resume all suspended thread */
    _ipc_list_resume_all(&(event->parent.suspend_thread));

    /* detach event object */
    phy_object_detach(&(event->parent.parent));

    return EOK;
}


phy_event_t phy_event_create(const char *name, uint8_t flag)
{
    phy_event_t event;

    /* allocate object */
    event = (phy_event_t)phy_object_allocate(phy_Object_Class_Event, name);
    if (event == NULL)
        return event;

    /* set parent */
    event->parent.parent.flag = flag;

    /* initialize ipc object */
    _ipc_object_init(&(event->parent));

    /* initialize event */
    event->eventSet = 0;

    return event;
}

err_t phy_event_delete(phy_event_t event)
{
    /* parameter check */
    ASSERT(event != NULL);
    ASSERT(phy_object_get_type(&event->parent.parent) == phy_Object_Class_Event);
    ASSERT(phy_object_is_systemobject(&event->parent.parent) == false);

  
    /* resume all suspended thread */
    _ipc_list_resume_all(&(event->parent.suspend_thread));

    /* delete event object */
    phy_object_delete(&(event->parent.parent));

    return EOK;
}

err_t PHY_event_send(phy_event_t event, uint32_t set)
{
     list_t *n;
    THREAD_t *thread;
    register phy_ubase_t level;
    register phy_base_t status;
    bool need_schedule;

    /* parameter check */
    ASSERT(event != NULL);
    ASSERT(phy_object_get_type(&event->parent.parent) == phy_Object_Class_Event);

    if (set == 0) return -ERROR;

    need_schedule = false;

    /* disable interrupt */
    level = phy_hw_interrupt_disable();

    /* set event */
    event->eventSet |= set;

    if (!list_is_empty(&event->parent.suspend_thread))
    {
        /* search thread list to resume thread */
        n = event->parent.suspend_thread.next;
        while (n != &(event->parent.suspend_thread))
        {
            /* get thread */
            thread = containerOf(n, THREAD_t, thdList);

            status = -ERROR;
            if (thread->event_info & PHY_EVENT_FLAG_AND)
            {
                if ((thread->event_set & event->eventSet) == thread->event_set)
                {
                    /* received an AND event */
                    status = EOK;
                }
            }
            else if (thread->event_info & PHY_EVENT_FLAG_OR)
            {
                if (thread->event_set & event->eventSet)
                {
                    /* save the received event set */
                    thread->event_set = thread->event_set & event->eventSet;

                    /* received an OR event */
                    status = EOK;
                }
            }
            else
            {
                /* enable interrupt */
                phy_hw_interrupt_enable(level);

                return -EINVAL;
            }

            /* move node to the next */
            n = n->next;

            /* condition is satisfied, resume thread */
            if (status == EOK)
            {
                /* clear event */
                if (thread->event_info & PHY_EVENT_FLAG_CLEAR)
                    event->eventSet &= ~thread->event_set;

                /* resume thread, and thread list breaks out */
                phy_thread_resume(thread);

                /* need do a scheduling */
                need_schedule = true;
            }
        }
    }

    /* enable interrupt */
    phy_hw_interrupt_enable(level);

    /* do a schedule */
    if (need_schedule == true)   start_sched();
       

    return EOK;
}


err_t phy_event_recv(phy_event_t   event,
                       uint32_t  set,
                       uint8_t   option,
                       int32_t   timeout,
                       uint32_t *recved)
{
    THREAD_t* thread= CURRENT_THREAD;
    register phy_ubase_t level;
    register phy_base_t status;

    /* parameter check */
    ASSERT(event != NULL);
    ASSERT(phy_object_get_type(&event->parent.parent) == phy_Object_Class_Event);

    if (set == 0)   return -ERROR;

    /* initialize status */
    status = -ERROR;
    
    /* reset thread error */
    thread->error = EOK;

    /* disable interrupt */
    level = phy_hw_interrupt_disable();

    /* check event set */
    if (option & PHY_EVENT_FLAG_AND)
    {
        if ((event->eventSet & set) == set)
            status = EOK;
    }  else if (option & PHY_EVENT_FLAG_OR)
    {
        if (event->eventSet & set)
            status = EOK;
    } else  {
        /* either phy_EVENT_FLAG_AND or phy_EVENT_FLAG_OR should be set */
        ASSERT(0);
    }

    if (status == EOK)
    {
        /* set received event */
        if (recved)
            *recved = (event->eventSet & set);

        /* fill thread event info */
        thread->event_set = (event->eventSet & set);
        thread->event_info = option;

        /* received event */
        if (option & PHY_EVENT_FLAG_CLEAR)
            event->eventSet &= ~set;
    }  else if (timeout == 0) {
        /* no waiting */
        thread->error = -ETIMEOUT;

        /* enable interrupt */
        phy_hw_interrupt_enable(level);

        return -ETIMEOUT;
    } else  {
        /* fill thread event info */
        thread->event_set  = set;
        thread->event_info = option;

        /* put thread to suspended thread list */
        _ipc_list_suspend(&(event->parent.suspend_thread),
                            thread,
                            event->parent.parent.flag);

        /* if there is a waiting timeout, active thread timer */
        if (timeout > 0)
        {
            /* reset the timeout of thread timer and start it */
          //  phy_timer_control(&(thread->thread_timer),
             //                PHY_TIMER_CTRL_SET_TIME,
               //              &timeout);
           // phy_timer_start(&(thread->thread_timer));

             addTimer(&(thread->thread_timer));
        }

    }

        /* enable interrupt */
        phy_hw_interrupt_enable(level);

        /* do a schedule */
       // phy_schedule();

        if (thread->error != EOK)
        {
            /* return error */
            return thread->error;
        }

        /* received an event, disable interrupt to protect */
        level = phy_hw_interrupt_disable();

        /* set received event */
        if (recved)
            *recved = thread->event_set;
    

    /* enable interrupt */
    phy_hw_interrupt_enable(level);
   
    return thread->error;
}