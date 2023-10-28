#pragma once

#include "task.h"
#include "semaphore.h"



#define phy_WAITING_FOREVER      -1       /**< Block forever until get resource. */
#define phy_WAITING_NO            0       /**< Non-block. */

/*! \struct LosMuxAttr */
typedef struct { //互斥锁的属性
    uint8_t protocol;		///< 协议
    uint8_t prioceiling;	///< 优先级上限
    uint8_t type;			///< 类型属性
    uint8_t reserved;		///< 保留字段
} phy_MuxAttr;

/**
 * Mutual exclusion (mutex) structure
 */
struct phy_mutex
{
    struct phy_ipc_object parent;                        /**< inherit from ipc_object */

    uint16_t          value;                         /**< value of mutex */

    uint8_t           original_priority;             /**< priority of last thread hold the mutex */
    uint8_t           hold;                          /**< numbers of thread hold the mutex */
    phy_MuxAttr      attr; 
    THREAD_t*    owner;                         /**< current owner of mutex */
};
typedef struct phy_mutex *phy_mutex_t;

phy_mutex_t phy_mutex_create(const char *name, uint8_t flag);
err_t phy_mutex_delete(phy_mutex_t mutex);
err_t phy_mutex_take(phy_mutex_t mutex, uint32_t time);
err_t phy_mutex_trytake(phy_mutex_t mutex);
err_t phy_mutex_release(phy_mutex_t mutex);

/*! \enum */
enum {
    PHY_MUX_PRIO_NONE = 0,		///< 线程的优先级和调度不会受到互斥锁影响,先来后到,普通排队.
    PHY_MUX_PRIO_INHERIT = 1,	///< 当高优先级的等待低优先级的线程释放锁时，低优先级的线程以高优先级线程的优先级运行。
    							///< 当线程解锁互斥量时，线程的优先级自动被将到它原来的优先级
    PHY_MUX_PRIO_PROTECT = 2	///<   OsMuxPendOp 
};

/*! \enum */
enum {
    PHY_MUX_NORMAL = 0,		///< 非递归锁 只有[0.1]两个状态,不做任何特殊的错误检,不进行deadlock detection(死锁检测)
    PHY_MUX_RECURSIVE = 1,	///< 递归锁 允许同一线程在互斥量解锁前对该互斥量进行多次加锁。递归互斥量维护锁的计数，在解锁次数和加锁次数不相同的情况下，不会释放锁，别的线程就无法加锁此互斥量。
    PHY_MUX_ERRORCHECK = 2,	///< 进行错误检查,如果一个线程企图对一个已经锁住的mutex进行relock或对未加锁的unlock，将返回一个错误。
    PHY_MUX_DEFAULT = PHY_MUX_RECURSIVE //系统默认使用递归锁
};

#define OS_TASK_PRIORITY_HIGHEST 0
#define OS_TASK_PRIORITY_LOWEST 31
