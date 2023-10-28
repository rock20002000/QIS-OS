#pragma once

#include "task.h"
#include "semaphore.h"



#define phy_WAITING_FOREVER      -1       /**< Block forever until get resource. */
#define phy_WAITING_NO            0       /**< Non-block. */

/*! \struct LosMuxAttr */
typedef struct { //������������
    uint8_t protocol;		///< Э��
    uint8_t prioceiling;	///< ���ȼ�����
    uint8_t type;			///< ��������
    uint8_t reserved;		///< �����ֶ�
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
    PHY_MUX_PRIO_NONE = 0,		///< �̵߳����ȼ��͵��Ȳ����ܵ�������Ӱ��,������,��ͨ�Ŷ�.
    PHY_MUX_PRIO_INHERIT = 1,	///< �������ȼ��ĵȴ������ȼ����߳��ͷ���ʱ�������ȼ����߳��Ը����ȼ��̵߳����ȼ����С�
    							///< ���߳̽���������ʱ���̵߳����ȼ��Զ���������ԭ�������ȼ�
    PHY_MUX_PRIO_PROTECT = 2	///<   OsMuxPendOp 
};

/*! \enum */
enum {
    PHY_MUX_NORMAL = 0,		///< �ǵݹ��� ֻ��[0.1]����״̬,�����κ�����Ĵ����,������deadlock detection(�������)
    PHY_MUX_RECURSIVE = 1,	///< �ݹ��� ����ͬһ�߳��ڻ���������ǰ�Ըû��������ж�μ������ݹ黥����ά�����ļ������ڽ��������ͼ�����������ͬ������£������ͷ���������߳̾��޷������˻�������
    PHY_MUX_ERRORCHECK = 2,	///< ���д�����,���һ���߳���ͼ��һ���Ѿ���ס��mutex����relock���δ������unlock��������һ������
    PHY_MUX_DEFAULT = PHY_MUX_RECURSIVE //ϵͳĬ��ʹ�õݹ���
};

#define OS_TASK_PRIORITY_HIGHEST 0
#define OS_TASK_PRIORITY_LOWEST 31
