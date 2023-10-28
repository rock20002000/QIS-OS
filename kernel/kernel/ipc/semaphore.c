#include "semaphore.h"
#include "ipc.h"
#include "stdarg.h"
#include "libc/include/iomsg.h"
#include "fcntl.h"
#include "errno.h"
#include "sched.h"

static sem_t *posix_sem_list = NULL;
static struct phy_semaphore posix_sem_lock;

void posix_sem_insert(sem_t *psem);

/* initialize posix semaphore */
static int posix_sem_system_init(void)
{
  
    phy_sem_init(&posix_sem_lock, "psem", 1, PHY_IPC_FLAG_FIFO);
    return 0;
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	//	char name[SEMA_NAME_MAX];
    //	static uint16_t psem_number = 0;
 
    if (sem == NULL)
    {
         return -1;
    }
   
     posix_sem_system_init();
   
    //	snprintf(name, sizeof(name), "psem%02d", psem_number++);
    //	DBG("\n sem->namePath:%s value:%d\n", sem->namePath,value);
    sem->sem = phy_sem_create(sem->namePath, value, PHY_IPC_FLAG_FIFO);
     
    if (sem->sem == NULL)
    {
     //   rt_set_errno(ENOMEM);
        return -1;
    }
      DBG("\n sem->sem->parent.parent.name:%s \n",
                              sem->sem->parent.parent.name );
    /* initialize posix semaphore */
    sem->refcount = 1;
    sem->unlinked = 0;
    sem->unamed = 0;
    
    /* lock posix semaphore list */
   // phy_sem_take(&posix_sem_lock, PHY_WAITING_FOREVER);
    phy_sem_take_notimer(&posix_sem_lock);
    posix_sem_insert(sem);
    phy_sem_release(&posix_sem_lock);

    return 0;
}

static sem_t *posix_sem_find(const char* name)
{
    sem_t *iter;
    phy_object_t object;

    for (iter = posix_sem_list; iter != NULL; iter = iter->next)
    {
        object = (phy_object_t)iter->sem;

        if (strncmp(object->name, name, PHY_NAME_MAX) == 0)
        {
            return iter;
        }
    }

    return NULL;
}
#if 0
err_t phy_thread_resume(THREAD_t* thread)
{  

    register phy_base_t temp;

    /* parameter check */
    ASSERT(thread != NULL);
    ASSERT(phy_object_get_type((phy_object_t)thread) == phy_Object_Class_Thread);

    DBG("thread resume:  %s\n", thread->name);

    if ((thread->state & THREAD_STATE_MASK) != THREAD_STATE_BLOCKED)
    {
        DBG("thread resume: thread disorder, %d\n",thread->state);

        return -ERROR;
    }

    /* disable interrupt */
    // irq_disable();

   //  phy_timer_stop(&thread->thread_timer);

     /* disable interrupt */
    temp = phy_hw_interrupt_disable();

    /* remove from suspend list */
    //list_remove(&(thread->thdList));
     thread_waitqueue_remove(thread);

   // rt_timer_stop(&thread->thread_timer);

    /* insert to schedule ready list */
    //phy_schedule_insert_thread(thread);
    thread_waitqueue_push(thread);

    /* enable interrupt */
   // irq_enable();
    phy_hw_interrupt_enable(temp);
    //RT_OBJECT_HOOK_CALL(rt_thread_resume_hook, (thread));
    return EOK;
}
#endif

err_t _ipc_list_resume(list_t *list)
{
    THREAD_t *thread;

    /* get thread entry */
   // thread = containerOf(list->next, THREAD_t, thdList);
    thread =    thread_waitqueue_pop();

    DBG("resume thread:%s\n", thread->name);

    /* resume it */
    phy_thread_resume(thread);

    return EOK;
}

inline void posix_sem_insert(sem_t *psem)
{
    psem->next = posix_sem_list;
    posix_sem_list = psem;
}

static void posix_sem_delete(sem_t *psem)
{
    sem_t *iter;
    if (posix_sem_list == psem)
    {
        posix_sem_list = psem->next;

        phy_sem_delete(psem->sem);

        if(psem->unamed == 0)
            free(psem);

        return;
    }
    for (iter = posix_sem_list; iter->next != NULL; iter = iter->next)
    {
        if (iter->next == psem)
        {
            /* delete this mq */
            if (psem->next != NULL)
                iter->next = psem->next;
            else
                iter->next = NULL;

            /* delete mqueue */
            phy_sem_delete(psem->sem);

            if(psem->unamed == 0)
                free(psem);

            return ;
        }
    }
}


int phy_sem_release(phy_sem_t sem)
{
    bool need_schedule;

    /* parameter check */
    ASSERT(sem != NULL);
    ASSERT(phy_object_get_type(&sem->parent.parent) == phy_Object_Class_Semaphore);

    need_schedule = false;

    /* disable interrupt */
     // irq_disable();
    // level = phy_hw_interrupt_disable();

    DBG("thread %s releases sem:%s, which value is: %d\n",
                                CURRENT_THREAD->name,
                                ((struct phy_object *)sem)->name,
                                sem->value);

        if(sem->value < PHY_SEM_VALUE_MAX)
        {
            sem->value ++; /* increase value */
        } else {
          //  irq_enable(); /* enable interrupt */
          //  phy_hw_interrupt_disable(level);
            return -EOVERFLOW; /* value overflowed */
        }
    

    /* enable interrupt */
  //  irq_enable();
    //   phy_hw_interrupt_disable(level);
    

    /* resume a thread, re-schedule */
    if (need_schedule == true)  start_sched();
  
    return sem->value;
}


int sem_close(sem_t *sem)
{
    if (sem == NULL)
    {
       // rt_set_errno(EINVAL);
        return -1;
    }

    /* lock posix semaphore list */
    phy_sem_take_notimer(&posix_sem_lock);
    sem->refcount --;
    if (sem->refcount == 0)
    {
        /* delete from posix semaphore list */
      //  if (sem->unlinked)
        posix_sem_delete(sem);
        sem = NULL;
    }
    phy_sem_release(&posix_sem_lock);

    return 0;
}

//((sem=sem_open(SEM_NAME,OFLAG,MODE,SEM_VALUE))==SEM_FAILED)
// open("/home/mycode/mycode.txt", O_RDWR  | O_CREAT | O_EXCL, 0644);

sem_t *sem_open(const char *name, int oflag, ...)
{
    sem_t* sem = NULL;
    va_list arg;
    unsigned int value;  
    /* lock posix semaphore list */
   // phy_sem_take(&posix_sem_lock, PHY_WAITING_FOREVER);
     phy_sem_take_notimer(&posix_sem_lock);
    if (oflag & O_CREAT)
    {
        va_start(arg, oflag);
        // short mode = (short) va_arg( arg, unsigned int);
        value = va_arg( arg, unsigned int);
        va_end(arg);

        if (oflag & O_EXCL)
        {
            if (posix_sem_find(name) != NULL)
            {
              //  rt_set_errno(EEXIST);
                goto __return;
            }
        }
        sem = (sem_t*) kmallocz (sizeof(struct posix_sem));
        if (sem == NULL)
        {
          //  rt_set_errno(ENFILE);
            goto __return;
        }

        /* create  semaphore */
        sem->sem = phy_sem_create(name, value, PHY_IPC_FLAG_FIFO);
        if (sem->sem == NULL) /* create failed */
        {
           // rt_set_errno(ENFILE);
            goto __return;
        }
        /* initialize reference count */
        sem->refcount = 1;
        sem->unlinked = 0;
        sem->unamed = 0;

        /* insert semaphore to posix semaphore list */
        posix_sem_insert(sem);
    }
    else
    {
        
        /* find semaphore */
        sem = posix_sem_find(name);
        
        if (sem != NULL)
        {
            sem->refcount ++; /* increase reference count */
            DBG(" sem->refcount ++: %d", sem->refcount ++);
        }
        else
        {
           // rt_set_errno(ENOENT);
            goto __return;
        }
    }
    phy_sem_release(&posix_sem_lock);
    
    return sem;

__return:
    /* release lock */
    phy_sem_release(&posix_sem_lock);

    /* release allocated memory */
    if (sem != NULL)
    {
        /* delete RT-Thread semaphore */
        if (sem->sem != NULL)
            phy_sem_delete(sem->sem);
        free(sem);
    }

    return NULL;
}


int sem_take(sem_t *sem)
{
    int result;

    if (!sem)
    {
     // rt_set_errno(EINVAL);
        return -1;
    }

    result = phy_sem_take_notimer(sem->sem);
    //if (result == EOK) return 0;
   // rt_set_errno(EINTR);

    return result;
}

int sem_trywait(sem_t *sem)
{
    err_t result;

    if (!sem)
    {
      //rt_set_errno(EINVAL);

        return -1;
    }

    result = phy_sem_take(sem->sem, 0);
    if (result == -ETIMEDOUT)
    {
      //  rt_set_errno(EAGAIN);

        return -1;
    }
    if (result == EOK)   return 0;

   // rt_set_errno(EINTR);

    return -1;
}

int sem_destroy(sem_t *sem)
{
    if ((!sem) || !(sem->unamed))
    {
      //  rt_set_errno(EINVAL);

        return -1;
    }

    /* lock posix semaphore list */
    phy_sem_take_notimer(&posix_sem_lock);
    if(list_len(&sem->sem->parent.suspend_thread) != 0)
    {
        phy_sem_release(&posix_sem_lock);
        //phy_set_errno(EBUSY);

        return -1;
    }

    /* destroy an unamed posix semaphore */
    posix_sem_delete(sem);
    phy_sem_release(&posix_sem_lock);

    return 0;
}

int sem_unlink(const char *name)
{
    sem_t *psem;

    /* lock posix semaphore list */
    phy_sem_take(&posix_sem_lock, PHY_WAITING_FOREVER);
    psem = posix_sem_find(name);
    if (psem != NULL)
    {
        psem->unlinked = 1;
        if (psem->refcount == 0)
        {
            /* remove this semaphore */
            posix_sem_delete(psem);
        }
        phy_sem_release(&posix_sem_lock);

        return 0;
    }
    phy_sem_release(&posix_sem_lock);

    /* no this entry */
    //rt_set_errno(ENOENT);

    return -1;
}

int sem_post(sem_t *sem)
{
    err_t result;

    if (!sem)
    {
        //rt_set_errno(EINVAL);
        return -1;
    }

    result = phy_sem_release(sem->sem);
   // if (result == EOK)  return 0;

    //rt_set_errno(EINVAL);
    return result;
}

int semValue(sem_t *sem, long *sval)
{
    if (!sem || !sval)
    {
      return -1;
    }
    *sval = (long)(sem->sem->value);

    return 0;
}
