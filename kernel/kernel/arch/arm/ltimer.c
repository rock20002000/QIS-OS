#include <types.h>
#include <interrupt.h>
#include <arch.h>
#include <apic.h>
#include <ltimer.h>
#include <debug.h>
#include <errno.h>
#include <locks.h>
#include "task.h"
#include "ipc.h"
#include "kobj.h"

 spinlock_t timerLock;
 list_t timersList;
 extern syspage_t* g_syspage_ptr;


int initTimer(Timer *timer, TimerCallBackFunction callback,
                     int timeout,void *data, size_t size, int fUserMode)
{ 
      /* timer object initialization */
    phy_object_init(&(timer->parent), phy_Object_Class_Timer, "Timer");
   
   /*This val `data` is an argument when we call `callback`.*/
   timer->callback = callback;
   timer->ticks = getTicks() + timeout; 
   timer->onStack = 0; 
      /*This field decides if we call kfree when this timer is timeout.*/
   if (fUserMode && data && size && !is_kernel_vaddr((vaddr_t)data))
   {
      data = copy_user_data(NULL, data, size);
   }
   timer->data = data;
   timer->size = size;
   timer->timeout = timeout;
   // initList(&timer->list);
   list_init(&timer->list);
   DBG("timer->list.prev:0x%p",timer->list.prev);
   return 0;
}

void free(void *ptr) ;
int removeTimer(Timer *timer)
{
 //  u64 rflags;
 //  lockSpinLockCloseInterrupt(&timerLock,&rflags);
     lock_acq(&timerLock);
   list_remove(&timer->list); 
      /*A list can be deleted more than one times.*/
      /*So this will never be failed.*/

   phy_object_detach(&(timer->parent));

   //unlockSpinLockRestoreInterrupt(&timerLock,&rflags);
    lock_rel(&timerLock);
   DBG("timer->onStack:%d \n", timer->onStack);
   if(!timer->onStack)
   {
      if (timer->data && is_kernel_vaddr((vaddr_t)timer->data))
                      kfree(timer->data);
        DBG("timer free :0x%p \n",timer);
      kfree(timer);
   }
   return 0;
}

err_t phy_timer_stop(Timer *timer)
{
    register phy_base_t level;

    /* timer check */
    ASSERT(timer != NULL);
   // RT_ASSERT(rt_object_get_type(&timer->parent) == RT_Object_Class_Timer);


    /* disable interrupt */
    level = phy_hw_interrupt_disable();

    //_rt_timer_remove(timer);
    removeTimer(timer);

    /* change status */
    //timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    /* enable interrupt */
    phy_hw_interrupt_enable(level);

    return EOK;
}

int addTimer(Timer *timer)
{
//   u64 rflags 		= 0;
   int fTimerAdded 	= 0;
   // lockSpinLockCloseInterrupt(&timerLock,&rflags);
   lock_acq(&timerLock);
   //DBG("%s \n", __func__);
   for(ListHead *list = timersList.next;list != &timersList;list = list->next)
   {
      //DBG("%s list:0x%p\n", __func__,list);
      Timer *__timer = listEntry(list,Timer,list);
      if(__timer->ticks > timer->ticks)
      {
    	  //Add this timer to a appropriate position.
    	  listAddTail(&timer->list,&__timer->list);
    	  fTimerAdded = 1;	//	goto end;
      }
   }

   //  DBG("%s \n", __func__);
   /*Add this timer to the last position.*/
   if(fTimerAdded==0) listAddTail(&(timer->list),&timersList);
   // DBG("%s \n", __func__);
   //	end:
   //   DBG("%s \n", __func__);
   //unlockSpinLockRestoreInterrupt(&timerLock,&rflags);
   lock_rel(&timerLock);
   //  DBG("%s \n", __func__);
   return 0;
}

int timeInterrupt(iframe_t* reg,void *data)
{
//   u64 rflags;
//   uint16_t cpuid = get_cpuvar()->cpu_id;
//paddr_t pageTableAddress = arch_pte_backup();
  //   DBG("%s \n",__func__);
    // arch_pte_restore(pageTableAddress);
   for(;;){
    //  lockSpinLockCloseInterrupt(&timerLock,&rflags);
         lock_acq(&timerLock);
      if(listEmpty(&timersList))
         break; /*Just break if empty.*/
      Timer *timer = listEntry(timersList.next,Timer,list);      
     
      if(timer->ticks > getTicks() ){ 
      //   DBG(" timer->ticks:%u CurTicks:%u  Break \n",timer->ticks, getTicks());  
         break; /*Break if timers are not timeout.*/
      }
      list_remove(&timer->list); /*Timeout!Delete it.*/
      
    //  unlockSpinLockRestoreInterrupt(&timerLock,&rflags);
     lock_rel(&timerLock);
     DBG("%s callback @@@@@@@@@@@@@@@@@@@@@  cur ticks:%u \n",__func__, getTicks());
   
      (*timer->callback)(timer->data); /*Call callback.*/

      if(timer->fValid){
    	  initTimer(timer, timer->callback, timer->timeout, timer->data, timer->size, 0);
    	  addTimer(timer);
      }else{
    	  removeTimer(timer);
      }
   }   

 //  unlockSpinLockRestoreInterrupt(&timerLock,&rflags);
     lock_rel(&timerLock);
      /*Need unlock.*/    

   // DBG("%s rflags:0x%p \n", __func__, rflags);

   return 0;
}

int initTimeList(void)
{
 //  if(initHpet(timeInterrupt,TICK_HZ))

   list_init(&timersList);
   spinlock_init(&timerLock);
   DBG("Initialize time successfully!\n\n");
   return 0;
}

Timer *createTimer(TimerCallBackFunction callback,int timeout,void *data, size_t size)
{
   //Timer *timer = (Timer *)kmalloc(sizeof(Timer));
   Timer *timer = (Timer *)kmallocz(sizeof(Timer));
   DBG(" timer->list:0x%p \n", timer->list);
   if (!timer) {
    DBG(" timer alloc failure"); 
    return NULL;
   }
   initTimer(timer, callback, timeout, data, size, 1);
   timer->onStack = 0; /*It will be free auto.*/
   DBG("CREATE SUCCESS timer->list:0x%p \n", &timer->list);
   return timer;
}

