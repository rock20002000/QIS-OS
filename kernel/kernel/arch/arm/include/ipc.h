#ifndef _ARM_IPC_H_
#define _ARM_IPC_H_
#include "task.h"

void ipc_copy_in(THREAD_t* p_thread, vaddr_t src, paddr_t dst, size_t size);

 phy_base_t phy_hw_interrupt_disable();
 void phy_hw_interrupt_enable(phy_base_t level);

 err_t phy_thread_resume(THREAD_t* thread);


 err_t _ipc_list_resume_all(list_t *list);

 err_t _ipc_list_resume(list_t *list);

err_t _ipc_list_suspend( list_t*      list,
                                THREAD_t*    thread,
                                uint8_t      flag);

err_t phy_thread_control(THREAD_t* thread, int cmd, void *arg);                                

#endif
