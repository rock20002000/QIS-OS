#pragma once
#include <types.h>
#include <utils/include/list.h>

#define PHY_NAME_MAX 64

/**
 * Base structure of Kernel object
 */
struct phy_object
{
    char    name[PHY_NAME_MAX];                      /**< name of kernel object */
    uint8_t type;                                    /**< type of kernel object */
    uint8_t flag;                                    /**< flag of kernel object */
    uint8_t szReserve[2];
    list_t  list;                                    /**< list node of kernel object */
};

typedef struct phy_object* phy_object_t;        
#define PHY_USING_SEMAPHORE
#define PHY_USING_MUTEX
#define PHY_USING_EVENT

/*
 * define object_info for the number of _object_container items.
 */
enum phy_object_info_type
{
    PHY_Object_Info_Thread = 0,                         /**< The object is a thread. */
#ifdef PHY_USING_SEMAPHORE
   PHY_Object_Info_Semaphore,                          /**< The object is a semaphore. */
#endif
#ifdef PHY_USING_MUTEX
    PHY_Object_Info_Mutex,                              /**< The object is a mutex. */
#endif
#ifdef PHY_USING_EVENT
    PHY_Object_Info_Event,                              /**< The object is a event. */
#endif
#ifdef PHY_USING_MAILBOX
    PHY_Object_Info_MailBox,                            /**< The object is a mail box. */
#endif
#ifdef PHY_USING_MESSAGEQUEUE
    PHY_Object_Info_MessageQueue,                       /**< The object is a message queue. */
#endif
#ifdef PHY_USING_MEMHEAP
    PHY_Object_Info_MemHeap,                            /**< The object is a memory heap */
#endif
#ifdef PHY_USING_MEMPOOL
    PHY_Object_Info_MemPool,                            /**< The object is a memory pool. */
#endif
#ifdef PHY_USING_DEVICE
    PHY_Object_Info_Device,                             /**< The object is a device */
#endif
    PHY_Object_Info_Timer,                              /**< The object is a timer. */
#ifdef PHY_USING_MODULE
    PHY_Object_Info_Module,                             /**< The object is a module. */
#endif
#ifdef PHY_USING_HEAP
    PHY_Object_Info_Memory,                            /**< The object is a memory. */
#endif
    PHY_Object_Info_Unknown,                            /**< The object is unknown. */
};

/**
 *  The object type can be one of the follows with specific
 *  macros enabled:
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum phy_object_class_type
{
    phy_Object_Class_Null          = 0x00,      /**< The object is not used. */
    phy_Object_Class_Thread        = 0x01,      /**< The object is a thread. */
    phy_Object_Class_Semaphore     = 0x02,      /**< The object is a semaphore. */
    phy_Object_Class_Mutex         = 0x03,      /**< The object is a mutex. */
    phy_Object_Class_Event         = 0x04,      /**< The object is a event. */
    phy_Object_Class_MailBox       = 0x05,      /**< The object is a mail box. */
    phy_Object_Class_MessageQueue  = 0x06,      /**< The object is a message queue. */
    phy_Object_Class_MemHeap       = 0x07,      /**< The object is a memory heap. */
    phy_Object_Class_MemPool       = 0x08,      /**< The object is a memory pool. */
    phy_Object_Class_Device        = 0x09,      /**< The object is a device. */
    phy_Object_Class_Timer         = 0x0a,      /**< The object is a timer. */
    phy_Object_Class_Module        = 0x0b,      /**< The object is a module. */
    phy_Object_Class_Memory        = 0x0c,      /**< The object is a memory. */
    phy_Object_Class_Unknown       = 0x0e,      /**< The object is unknown. */
    phy_Object_Class_Static        = 0x80       /**< The object is a static object. */
};

/**
 * The information of the kernel object
 */
struct phy_object_information
{
    enum phy_object_class_type type;                 /**< object class type */
    list_t                 object_list;              /**< object list */
    size_t                 object_size;              /**< object size */
};

struct phy_object_information* phy_object_get_information(enum phy_object_class_type type);

uint8_t phy_object_get_type(phy_object_t object);
