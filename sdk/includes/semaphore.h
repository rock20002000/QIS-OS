#pragma once

#include <types.h>
#include <utils/include/list.h>
#include <object.h>


/* maximum value of ipc type */
#define PHY_SEM_VALUE_MAX                UINT16_MAX   /**< Maximum number of semaphore .value */
#define PHY_MUTEX_VALUE_MAX              UINT16_MAX   /**< Maximum number of mutex .value */
#define PHY_MUTEX_HOLD_MAX               UINT8_MAX    /**< Maximum number of mutex .hold */
#define PHY_MB_ENTRY_MAX                 UINT16_MAX   /**< Maximum number of mailbox .entry */
#define PHY_MQ_ENTRY_MAX                 UINT16_MAX   /**< Maximum number of message queue .entry */

/**
 * thread control command definitions
 */
#define PHY_THREAD_CTRL_STARTUP          0x00                /**< Startup thread. */
#define PHY_THREAD_CTRL_CLOSE            0x01                /**< Close thread. */
#define PHY_THREAD_CTRL_CHANGE_PRIORITY  0x02                /**< Change thread priority. */
#define PHY_THREAD_CTRL_INFO             0x03                /**< Get thread information. */
#define PHY_THREAD_CTRL_BIND_CPU         0x04                /**< Set thread bind cpu. */


	struct _sync_attr { 
		int		__protocol; 
		int		__flags; 
		int		__prioceiling;	/* Not implemented */ 
		int		__clockid;		/* Condvars only */ 
		int		__reserved[4]; 
	};

    enum{
       _NTO_SYNC_SEM,     

    };

/**
 * IPC flags and control command definitions
 */
#define PHY_IPC_FLAG_FIFO                0x00            /**< FIFOed IPC. @ref IPC. */
#define PHY_IPC_FLAG_PRIO                0x01            /**< PRIOed IPC. @ref IPC. */

struct phy_ipc_object
{
    struct phy_object parent;                  /**< inherit from rt_object */

    list_t holdList;   /**< The task holding the lock change | 当有任务拿到本锁时,通过holdList节点把锁挂到该任务的锁链表上 */

    list_t  suspend_thread;                    /**< threads pended on this resource */
};

struct phy_semaphore
{
    struct phy_ipc_object parent;                        /**< inherit from ipc_object */

    uint16_t          value;                         /**< value of semaphore. */
    uint16_t          reserved;                      /**< reserved field */
};
typedef struct phy_semaphore* phy_sem_t;

struct posix_sem
{
    /* reference count and unlinked */
    uint16_t refcount;
    uint8_t unlinked;
    uint8_t unamed;

    /* semaphore */
    phy_sem_t sem;
    phys_addr_t sempHandler;
    char namePath[32];

    /* next posix semaphore */
    struct posix_sem* next;
};

typedef struct posix_sem sem_t;

int sem_close(sem_t *sem);
int sem_destroy(sem_t *sem);

int sem_init(sem_t *sem, int pshared, unsigned int value);
sem_t *semaphoreOpen(const char *name,int size, int oflag, ...);
int sem_post(sem_t *sem);
//int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_trywait(sem_t *sem);
int sem_unlink(const char *name);
int sem_wait(sem_t *sem);
int sem_take(sem_t *sem);
int phy_sem_take_notimer(phy_sem_t sem);

#define SEMA_NAME_MAX 8

#define PHY_WAITING_FOREVER              -1              /**< Block forever until get resource. */
#define PHY_WAITING_NO                   0               /**< Non-block. */

typedef struct phy_semaphore* phy_sem_t;
phy_sem_t phy_sem_create(const char *name, uint32_t value, uint8_t flag);
err_t phy_sem_take(phy_sem_t sem, size_t timeout);
err_t phy_sem_init(phy_sem_t sem, const char *name, uint32_t value, uint8_t  flag);
int phy_sem_release(phy_sem_t sem);
bool phy_object_is_systemobject(phy_object_t object);
void phy_object_delete(phy_object_t object);
int phy_sem_delete(phy_sem_t sem);
sem_t *sem_open(const char *name, int oflag, ...);
int semValue(sem_t *sem, long *sval);

int semGetvalue(sem_t *sem);
int SyncTypeTake(sem_t* semp);
int SyncDestroy(sem_t* semp);
 int SyncSemPost(sem_t* semp);
 int SyncSemValue(sem_t* semp,  long* sval);
