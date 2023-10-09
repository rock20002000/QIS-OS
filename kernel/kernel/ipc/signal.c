#include "signal.h"
#include "task.h"
#include "stdbool.h"
#include <libc/include/iomsg.h>


void phy_hw_context_switch_to(phy_ubase_t to);

void phy_thread_handle_sig(bool clean_state)
{
    phy_base_t level;

    THREAD_t*  tid= CURRENT_THREAD;
    struct siginfo_node *si_node;

    level = phy_hw_interrupt_disable();

    if (tid->sig_pending & tid->sig_mask)
    {

        /* if thread is not waiting for signal */
        if (!(tid->state & PHY_THREAD_STAT_SIGNAL_WAIT))
        {
             while (tid->sig_pending & tid->sig_mask)
            {
                int signo, error;
                phy_sighandler_t handler;

                si_node = (struct siginfo_node *)tid->si_list;
                if (!si_node) break;

                       /* remove this sig info node from list */
                if (si_node->list.next == NULL)
                    tid->si_list = NULL;
                else
                    tid->si_list = (void *)containerOf(si_node->list.next, struct siginfo_node, list);

                signo   = si_node->si.si_signo;
                handler = tid->sig_vectors[signo];
                tid->sig_pending &= ~sig_mask(signo);

                phy_hw_interrupt_enable(level);

                printf("handle signal: %d, handler 0x%08x", signo, handler);
                if (handler) handler(signo);

                level = phy_hw_interrupt_disable();
                error = -PHY_EINTR;

                 /* set errno in thread tcb */
                tid->error = error;

                free(si_node);

            }

        }

    }
    return;
}

#if 0
static void _signal_entry(void *parameter)
{
   // THREAD_t tid = rt_thread_self();
    THREAD_t*  tid= CURRENT_THREAD;

    /* handle signal */
    phy_thread_handle_sig(false);

#ifdef PHY_USING_SMP
    {
        struct rt_cpu* pcpu = rt_cpu_self();

        pcpu->current_thread->cpus_lock_nest--;
        if (pcpu->current_thread->cpus_lock_nest == 0)
        {
            pcpu->current_thread->scheduler_lock_nest--;
        }

    }
#else
    /* return to thread */
    tid->sp = tid->sig_ret;
    tid->sig_ret = NULL;
#endif /* RT_USING_SMP */

    printf("switch back to: 0x%08x\n", tid->sp);
    tid->state &= ~PHY_THREAD_STAT_SIGNAL;

#ifdef PHY_USING_SMP
    rt_hw_context_switch_to((rt_base_t)&parameter, tid);
#else
    phy_hw_context_switch_to((phy_base_t)&(tid->sp));
#endif /* RT_USING_SMP */
}
#endif

volatile uint8_t phy_interrupt_nest = 0;

uint8_t phy_interrupt_get_nest(void)
{
    uint8_t ret;
    phy_base_t level;

    level = phy_hw_interrupt_disable();
    ret = phy_interrupt_nest;
    phy_hw_interrupt_enable(level);
    return ret;
}


static void _signal_deliver(THREAD_t* tid)
{
     phy_ubase_t level;

      tid= CURRENT_THREAD;

      level = phy_hw_interrupt_disable();

    /* thread is not interested in pended signals */
    if (!(tid->sig_pending & tid->sig_mask))
    {
        phy_hw_interrupt_enable(level);
        return;
    }

    
    if ((tid->state & PHY_THREAD_STAT_SIGNAL_MASK) == PHY_THREAD_STAT_SIGNAL_PENDING)
    {
        /* resume thread to handle signal */
        phy_thread_resume(tid);
        /* add signal state */
        tid->state |= (PHY_THREAD_STAT_SIGNAL | PHY_THREAD_STAT_SIGNAL_PENDING);

        phy_hw_interrupt_enable(level);

        /* re-schedule */
      //  rt_schedule();
    } else{

        if (tid == CURRENT_THREAD )
        {
            /* add signal state */
            tid->state |= PHY_THREAD_STAT_SIGNAL;

            phy_hw_interrupt_enable(level);

            /* do signal action in self thread context */
            if (phy_interrupt_get_nest() == 0)
            {
                phy_thread_handle_sig(true);
            }
        }else if (!((tid->state & PHY_THREAD_STAT_SIGNAL_MASK) & PHY_THREAD_STAT_SIGNAL))
        {
            /* add signal state */
            tid->state |= (PHY_THREAD_STAT_SIGNAL | PHY_THREAD_STAT_SIGNAL_PENDING);

             /* point to the signal handle entry */
            tid->state &= ~PHY_THREAD_STAT_SIGNAL_PENDING;
            tid->sig_ret = tid->sp;
            //hw_stack_init
            
            phy_hw_interrupt_enable(level);
            printf("signal stack pointer @ 0x%08x", tid->sp);

            /* re-schedule */
            //start_sched();
        }else  phy_hw_interrupt_enable(level);
   }
}

void phy_thread_alloc_sig(THREAD_t* tid)
{
    int index;
    phy_base_t level;
    phy_sighandler_t *vectors;

    vectors = (phy_sighandler_t *)kmallocz(sizeof(phy_sighandler_t) * PHY_SIG_MAX);
   // RT_ASSERT(vectors != NULL);

    for (index = 0; index < PHY_SIG_MAX; index ++)
    {
        vectors[index] = _signal_default_handler;
    }

    level = phy_hw_interrupt_disable();
    tid->sig_vectors = vectors;
    phy_hw_interrupt_enable(level);
}

phy_sighandler_t phy_signal_install(int signo, phy_sighandler_t handler)
{
    phy_base_t level;
    phy_sighandler_t old = NULL;
    THREAD_t* tid = CURRENT_THREAD;

  //  if (!sig_valid(signo)) return SIG_ERR;

    level = phy_hw_interrupt_disable();
    if (tid->sig_vectors == NULL)
    {
        phy_thread_alloc_sig(tid);
    }

    if (tid->sig_vectors)
    {
        old = tid->sig_vectors[signo];

        if (handler == SIG_IGN) tid->sig_vectors[signo] = NULL;
        else if (handler == SIG_DFL) tid->sig_vectors[signo] = _signal_default_handler;
        else tid->sig_vectors[signo] = handler;
    }
    phy_hw_interrupt_enable(level);

    return old;
}

int phy_thread_kill(THREAD_t* tid, int sig)
{
    siginfo_t si;
    phy_base_t level;
    struct siginfo_node *si_node;

    ASSERT(tid != NULL);

    printf("send signal: %d", sig);
    si.si_signo = sig;
    si.si_code  = SI_USER;
    //si.si_value.sival_ptr = NULL;

    level = phy_hw_interrupt_disable();

    if (tid->sig_pending & sig_mask(sig))
    {
        /* whether already emits this signal? */
        list_t *node;
        struct siginfo_node  *entry;

        si_node = (struct siginfo_node *)tid->si_list;
        if (si_node)  node = (list_t *)&si_node->list;
        else   node = NULL;

         /* update sig info */
        for (; (node) != NULL; node = node->next)
        {
            entry = containerOf(node, struct siginfo_node, list);
            if (entry->si.si_signo == sig)
            {
                memcpy(&(entry->si), &si, sizeof(siginfo_t));
                phy_hw_interrupt_enable(level);
                return 0;
            }
        }
    }

       phy_hw_interrupt_enable(level);

       /* deliver signal to this thread */
    _signal_deliver(tid);

    return EOK;
}

void phy_signal_mask(int signo)
{
    phy_base_t level;
    THREAD_t* tid = CURRENT_THREAD;

    level = phy_hw_interrupt_disable();

    tid->sig_mask &= ~sig_mask(signo);

    phy_hw_interrupt_enable(level);
}

void phy_signal_unmask(int signo)
{
    phy_base_t level;
    THREAD_t* tid = CURRENT_THREAD;

    level = phy_hw_interrupt_disable();

    tid->sig_mask |= sig_mask(signo);

    /* let thread handle pended signals */
    if (tid->sig_mask & tid->sig_pending)
    {
        phy_hw_interrupt_enable(level);
        _signal_deliver(tid);
    }
    else
    {
        phy_hw_interrupt_enable(level);
    }
}

int phy_signal_wait(const phy_sigset_t *set, phy_siginfo_t *si, int32_t timeout)
{
    int ret = EOK;

    /* clear siginfo to avoid unknown value */
    memset(si, 0x0, sizeof(phy_siginfo_t));
    
    phy_hw_interrupt_disable();

    return ret;
}
