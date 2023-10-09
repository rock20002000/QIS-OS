#include <semaphore.h>
#include <utils/include/list.h>
#include <object.h>
#include <task.h>
#include <sched.h>
#include <types.h>
#include <mutex.h>
#include <event.h>
#include "ipc.h"


void ipc_copy_in(THREAD_t* p_thread, vaddr_t src, paddr_t dst, size_t size);

 extern phy_base_t phy_hw_interrupt_disable();
 extern void phy_hw_interrupt_enable(phy_base_t level);


 err_t _ipc_list_resume_all(list_t *list);

err_t _ipc_list_suspend( list_t*      list,
                                THREAD_t*    thread,
                                uint8_t      flag);

err_t phy_thread_control(THREAD_t* thread, int cmd, void *arg);        

#define _OBJ_CONTAINER_LIST_INIT(c)     \
    {&(_object_container[c].object_list), &(_object_container[c].object_list)}

static struct phy_object_information _object_container[PHY_Object_Info_Unknown] =
{
    /* initialize object container - thread */
    {phy_Object_Class_Thread, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Thread), sizeof( THREAD_t) },
#ifdef PHY_USING_SEMAPHORE
    /* initialize object container - semaphore */
    {phy_Object_Class_Semaphore, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Semaphore), sizeof(struct phy_semaphore)},
#endif
#ifdef PHY_USING_MUTEX
    /* initialize object container - mutex */
    {phy_Object_Class_Mutex, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Mutex), sizeof(struct phy_mutex)},
#endif
#ifdef PHY_USING_EVENT
    /* initialize object container - event */
    {phy_Object_Class_Event, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Event), sizeof(struct phy_event)}, 
#endif
#ifdef PHY_USING_MAILBOX
    /* initialize object container - mailbox */
    {phy_Object_Class_MailBox, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_MailBox), sizeof(struct PHY_mailbox)},
#endif
#ifdef PHY_USING_MESSAGEQUEUE
    /* initialize object container - message queue */
    {phy_Object_Class_MessageQueue, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_MessageQueue), sizeof(struct PHY_messagequeue)},
#endif
#ifdef PHY_USING_MEMHEAP
    /* initialize object container - memory heap */
    {phy_Object_Class_MemHeap, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_MemHeap), sizeof(struct PHY_memheap)},
#endif
#ifdef PHY_USING_MEMPOOL
    /* initialize object container - memory pool */
    {phy_Object_Class_MemPool, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_MemPool), sizeof(struct PHY_mempool)},
#endif
#ifdef PHY_USING_DEVICE
    /* initialize object container - device */
    {PHY_Object_Class_Device, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Device), sizeof(struct PHY_device)},
#endif
    /* initialize object container - timer */
    {phy_Object_Class_Timer, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Timer), sizeof(struct phy_timer)},
#ifdef PHY_USING_MODULE
    /* initialize object container - module */
    {phy_Object_Class_Module, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Module), sizeof(struct PHY_dlmodule)},
#endif
#ifdef PHY_USING_HEAP
    /* initialize object container - small memory */
    {phy_Object_Class_Memory, _OBJ_CONTAINER_LIST_INIT(PHY_Object_Info_Memory), sizeof(struct PHY_memory)},
#endif
};


struct phy_object_information* phy_object_get_information(enum phy_object_class_type type)
{
    int index;

    for (index = 0; index < PHY_Object_Info_Unknown; index ++)
        if (_object_container[index].type == type) return &_object_container[index];

    return NULL;
}

uint8_t phy_object_get_type(phy_object_t object)
{
    /* object check */
    ASSERT(object != NULL);

    return object->type & ~phy_Object_Class_Static;
}

bool phy_object_is_systemobject(phy_object_t object)
{
    /* object check */
    ASSERT(object != NULL);

    if (object->type & phy_Object_Class_Static)
        return true;

    return false;
}

void phy_object_delete(phy_object_t object)
{
    /* object check */
    ASSERT(object != NULL);
    ASSERT(!(object->type & phy_Object_Class_Static));

    //RT_OBJECT_HOOK_CALL(rt_object_detach_hook, (object));

    /* reset object type */
    object->type = phy_Object_Class_Null;

    /* lock interrupt */
   // irq_disable();
  //  level = phy_hw_interrupt_disable();

    /* remove from old list */
    list_remove(&(object->list));

    DBG("Object->type:%d release\n",phy_object_get_type(object));

    /* unlock interrupt */
  //  irq_enable();
   //  phy_hw_interrupt_enable(level);


    /* free the memory of object */
    free(object);
}

void phy_object_init(struct phy_object         *object,
                    enum phy_object_class_type type,
                    const char                 *name)
{
    list_t *node = NULL;
    struct phy_object_information *information;
    
    /* get object information */
    information = phy_object_get_information(type);
    ASSERT(information != NULL);
   
    /* check object type to avoid re-initialization */

    /* enter critical */
   // phy_enter_critical();
   
    /* try to find object */
    for (node  = information->object_list.next;
            node != &(information->object_list);
            node  = node->next)
    {
        struct phy_object *obj;

        obj = containerOf(node, struct phy_object, list);
        if (obj) /* skip warning when disable debug */
        {
           // ASSERT(obj != object);
           if (obj == object) break;
        }
    }
    
    /* leave critical */
   // phy_exit_critical();
 
    /* initialize object's parameters */
    /* set object type to static */
    object->type = type | phy_Object_Class_Static;
    /* copy name */
    strncpy(object->name, name, PHY_NAME_MAX);
 
    //RT_OBJECT_HOOK_CALL(rt_object_attach_hook, (object));

    /* lock interrupt */
  //  irq_disable();
    // level = phy_hw_interrupt_disable();
 
    /* insert object into information object list */
    list_insert_after(&(information->object_list), &(object->list));
 
    /* unlock interrupt */
  //  irq_enable();
 //  phy_hw_interrupt_enable(level);
}

void phy_object_detach(phy_object_t object)
{
    /* object check */
    ASSERT(object != NULL);

      /* reset object type */
    object->type = 0;

    /* lock interrupt */
   // temp = phy_hw_interrupt_disable();

    /* remove from old list */
    list_remove(&(object->list));

    /* unlock interrupt */
  //  phy_hw_interrupt_enable(temp);
}
