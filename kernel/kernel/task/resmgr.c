/*
 * ky_resmgr.c
 *
 *  Created on: 2022��7��25��
 *      Author: Test
 */
#include <utils/include/dyn_array.h> 


#include "task.h"

#include "arch_task.h"
#include "kernel.h"
#include "aether_resmgr.h"
#include <ker_syscalls.h>
//#include "liblist.h"
//#include <priv.h>
//#include "../task/include/task.h"
//#include "sys/ky_states.h"

DYNARRAY_STRUCT(DEVICE_NODE *) 				device_node_lookup_lists[MAX_DEVICE_TYPE_SUPPORT];

DYNARRAY_STRUCT(Timer* )					timer_event_lookup_list;

DYNARRAY_STRUCT(struct sigevent * )			interrupt_event_lookup_list[MAX_INTERRUPT_SUPPORT];

////////////////////////////////////////////////////////////////////////
DYNARRAY_STRUCT(THREAD_t *) 				thread_fifo_running_lists[MAX_PRIORITYS];			//	The highest priority system blocking threads to be run
DYNARRAY_STRUCT(THREAD_t *) 				thread_fifo_waiting_lists[MAX_PRIORITYS];

DYNARRAY_STRUCT(THREAD_t *) 				thread_privilege_lists[MAX_PRIORITYS];		//	The privileged threads to be run

DYNARRAY_STRUCT(THREAD_t *) 				thread_runing_lists[MAX_PRIORITYS];			//	The normal running threads

DYNARRAY_STRUCT(THREAD_t *) 				thread_waiting_lists[MAX_PRIORITYS];		//	The threads just created waiting to be scheduled to run

DYNARRAY_STRUCT(THREAD_t *) 				thread_destroy_lists[MAX_PRIORITYS];		//	The threads be destroy



//	DYNARRAY_STRUCT(PROCESS_t *) 				process_created_queue;						//	The processes created

DYNARRAY_STRUCT(PROCESS_t *) 				process_lookup_lists[MAX_PROCESS_SUPPORT];	//	The processes look up list with page table address

DYNARRAY_STRUCT(RESOURCE_DEVICE *) 			resource_device_lookup_lists[MAX_RESOURCE_DEVICE_SUPPORT];

DYNARRAY_STRUCT(FILE_DESCRIPTOR *) 			file_descriptor_lookup_lists[MAX_PROCESS_SUPPORT];	//	MAX_RESOURCE_DEVICE_SUPPORT];

DYNARRAY_STRUCT(MSG_WAIT_REPLY *) 			msg_wait_reply_lookup_lists[MAX_PROCESS_SUPPORT];	//	MAX_RESOURCE_DEVICE_SUPPORT];

DYNARRAY_STRUCT(THREAD_t *) 				interrupt_wait_queue_lookup_lists[MAX_INTERRUPT_SUPPORT];

DYNARRAY_STRUCT(SLEEP_THREAD_WAIT_WAKEUP *) sleep_wait_queue_lookup_lists[MAX_SLEEP_TYPE_SUPPORT];

DYNARRAY_STRUCT(AETHER_DESC *) 				aether_queue_lookup_lists[MAX_PROCESS_SUPPORT];

DYNARRAY_STRUCT(AETHER_CONNECTION_DESC *) 	aether_connection_queue_lookup_lists;

DYNARRAY_STRUCT(int ) 						unique_random_int_queue_lookup_lists;

DYNARRAY_STRUCT(pthread_mutex_desc *) 		sync_wait_queue_lookup_lists[MAX_SYNC_SUPPORT];

//	DYNARRAY_STRUCT(THREAD_t *) 				device_wait_queue_lookup_lists[MAX_SLEEP_TYPE_SUPPORT];
//	DYNARRAY_STRUCT(THREAD_t *) 				device_ready_queue_lookup_lists[MAX_INTERRUPT_SUPPORT];
//	DYNARRAY_STRUCT(THREAD_t *) 				normal_ready_queue_lookup_lists[MAX_INTERRUPT_SUPPORT];


void resmgr_init()
{
    for(int i = 0; i<MAX_DEVICE_TYPE_SUPPORT ; i++)
    {
    	DYNARRAY_NULLIFY(device_node_lookup_lists[i]);
    }

//////////////////////////////////////////////////

    for(int i = 0; i < MAX_RESOURCE_DEVICE_SUPPORT; i++)
    {
        DYNARRAY_NULLIFY(resource_device_lookup_lists[i]);

        DYNARRAY_NULLIFY(file_descriptor_lookup_lists[i]);

        DYNARRAY_NULLIFY(msg_wait_reply_lookup_lists[i]);
    }

    for(int i = 0; i < MAX_INTERRUPT_SUPPORT; i++)
    {
    	DYNARRAY_NULLIFY(interrupt_wait_queue_lookup_lists[i]);
    }

    for(int i = 0; i < MAX_SLEEP_TYPE_SUPPORT; i++)
    {
    	DYNARRAY_NULLIFY(sleep_wait_queue_lookup_lists[i]);
    }

    for(int i = 0; i<MAX_PROCESS_SUPPORT ; i++)
    {
    	DYNARRAY_NULLIFY(aether_queue_lookup_lists[i]);
    }


    DYNARRAY_NULLIFY(aether_connection_queue_lookup_lists);

    DYNARRAY_NULLIFY(unique_random_int_queue_lookup_lists);

    for(int i = 0; i<MAX_SYNC_SUPPORT ; i++)
    {
    	DYNARRAY_NULLIFY(sync_wait_queue_lookup_lists[i]);
    }

    DYNARRAY_NULLIFY(timer_event_lookup_list);


    for(int i = 0; i<MAX_INTERRUPT_SUPPORT ; i++)
    {
    	DYNARRAY_NULLIFY(interrupt_event_lookup_list[i]);
    }


}


uint32_t resmgr_get_unique_random_int(int fPosValue){
	uint32_t iTmp 	= 0;
	int fValid 	= 0;

	while( fValid == 0 ){
		fValid 	= 1;
		iTmp 	= get_random_int();
		if(fPosValue){
			if(iTmp&0x80000000){
				iTmp &= 0x7fffffff;
				//	fValid = 0;
				//	continue;
			}
		}
		for(int i=0; i<unique_random_int_queue_lookup_lists.length; i++){
			if(unique_random_int_queue_lookup_lists.storage[i] == iTmp){
				fValid = 0;
				break;
			}
		}
	}

	DYNARRAY_PUSHBACK(unique_random_int_queue_lookup_lists, iTmp);
	return iTmp;
}


////////////////////////////////////////////////////////////////////////////////////

size_t interrupt_event_lookup_lists_push(int iIrq, struct sigevent * pSigEventTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = iIrq;
    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(interrupt_event_lookup_list[listPosition], pSigEventTmp);
    // KDBG("The position for the %d interrupt_event_lookup_list:%d \n", listPosition, interrupt_event_lookup_list[listPosition].length);
    return idx;
}


struct sigevent * lookupInterruptEventLookupListsWithIrq(int iIrq){
	uint16_t listPosition = iIrq;
	if(interrupt_event_lookup_list[listPosition].length == 0) return NULL;
	if(interrupt_event_lookup_list[listPosition].length >= 1) return interrupt_event_lookup_list[listPosition].storage[0];

	return NULL;	//	This should never reached
}

int getLookupInterruptEventLookupListsLength(int iIrq){
	uint16_t listPosition = iIrq;
	return interrupt_event_lookup_list[listPosition].length;
}
struct sigevent * lookupInterruptEventLookupListsWithIrqIndex(int iIrq, int iIndex){
	uint16_t listPosition = iIrq;
	if(interrupt_event_lookup_list[listPosition].length == 0) return NULL;
	if(interrupt_event_lookup_list[listPosition].length >= 1)
	{
		if(iIndex<interrupt_event_lookup_list[listPosition].length){
			return interrupt_event_lookup_list[listPosition].storage[iIndex];
		}else{
			return interrupt_event_lookup_list[listPosition].storage[0];
		}

	}

	return NULL;	//	This should never reached
}

void interrupt_event_lookup_lists_remove(int iIrq, struct sigevent * pSigEventTmp)
{
	uint16_t listPosition = iIrq;

    //ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(interrupt_event_lookup_list[listPosition], pSigEventTmp);
}

size_t timer_event_queue_lookup_lists_push(Timer* pSigEvent)
{
	if(pSigEvent==NULL) return -1;
    size_t idx = DYNARRAY_PUSHBACK(timer_event_lookup_list, pSigEvent);
    //	KDBG("The position for the timer_event_lookup_list:%d \n", listPosition, timer_event_lookup_list.length);
    return idx;
}

Timer* lookupTimerEventQueueLookupListsWithTimerID(int iTimerIDTmp)
{
	if(timer_event_lookup_list.length == 0) return NULL;
	if(timer_event_lookup_list.length >= 1){
		for(int iTimerEventIndex=0; iTimerEventIndex<timer_event_lookup_list.length; iTimerEventIndex++){
			if(timer_event_lookup_list.storage[iTimerEventIndex]->iTimerID == iTimerIDTmp ){
				return timer_event_lookup_list.storage[iTimerEventIndex];
			}
		}

	}

	return NULL;	//	This should never reached
}

int getTimerEventQueueLookupListsLength(){
	return timer_event_lookup_list.length;
}

void timer_event_queue_lookup_lists_remove(Timer* pSigEvent)
{
	if(pSigEvent==NULL) return;

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(timer_event_lookup_list, pSigEvent);
}


size_t receive_msg_block_queue_lookup_lists_push(AETHER_DESC* aetherDescTmp, MSG_SENDRECV_BLOCK_DESC* threadRecvMsgBlocking)
{
	if(aetherDescTmp==NULL) return -1;
    size_t idx = DYNARRAY_PUSHBACK(aetherDescTmp->recvMsgBlockThreadsQueue, threadRecvMsgBlocking);
    //	KDBG("The position for the receive_block_queue:%d \n", listPosition, interrupt_wait_queue_lookup_lists[listPosition].length);
    return idx;
}


MSG_SENDRECV_BLOCK_DESC* lookupReceiveMsgBlockQueueLookupLists(AETHER_DESC* aetherDescTmp){
	if(aetherDescTmp==NULL) return NULL;

	if(aetherDescTmp->recvMsgBlockThreadsQueue.length == 0) return NULL;
	if(aetherDescTmp->recvMsgBlockThreadsQueue.length >= 1) return aetherDescTmp->recvMsgBlockThreadsQueue.storage[0];

	return NULL;	//	This should never reached
}

void receive_msg_block_queue_lookup_lists_remove(AETHER_DESC* aetherDescTmp, MSG_SENDRECV_BLOCK_DESC* threadRecvBlocking)
{
	if(aetherDescTmp==NULL) return;

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aetherDescTmp->recvMsgBlockThreadsQueue, threadRecvBlocking);
}


size_t receive_pulse_block_queue_lookup_lists_push(AETHER_DESC* aetherDescTmp, PULSE_RECV_BLOCK_DESC* threadRecvPulseBlocking)
{
	if(aetherDescTmp==NULL) return -1;
    size_t idx = DYNARRAY_PUSHBACK(aetherDescTmp->recvPulseBlockThreadsQueue, threadRecvPulseBlocking);
    //	KDBG("The position for the receive_block_queue:%d \n", listPosition, interrupt_wait_queue_lookup_lists[listPosition].length);
    return idx;
}


PULSE_RECV_BLOCK_DESC* lookupReceivePulseBlockQueueLookupLists(AETHER_DESC* aetherDescTmp){
	if(aetherDescTmp==NULL) return NULL;

	if(aetherDescTmp->recvPulseBlockThreadsQueue.length == 0) return NULL;
	if(aetherDescTmp->recvPulseBlockThreadsQueue.length >= 1) return aetherDescTmp->recvPulseBlockThreadsQueue.storage[0];

	return NULL;	//	This should never reached
}

void receive_pulse_block_queue_lookup_lists_remove(AETHER_DESC* aetherDescTmp, PULSE_RECV_BLOCK_DESC* threadRecvPulseBlocking)
{
	if(aetherDescTmp==NULL) return;

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aetherDescTmp->recvPulseBlockThreadsQueue, threadRecvPulseBlocking);
}


///////////////////////////////////////////////////////////////////////////////

size_t device_node_lookup_lists_push(DEVICE_NODE* pDeviceNodeTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = 0;	//resDevTmp->resdevToken[0]<<8 | resDevTmp->resdevToken[1];	//(proc->arch.mmu.pml4&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(device_node_lookup_lists[listPosition], pDeviceNodeTmp);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    // KDBG("The position for the %d device_node_lookup_lists:%d \n", listPosition, device_node_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
DEVICE_NODE* lookupDeviceNodeLookupListsWithDevicePath(const char* pszDevicePathTmp){
	uint16_t listPosition = 0;	//devTokens;	//(devTokens&0xFFFF0000)>>16;
	if(device_node_lookup_lists[listPosition].length == 0) return NULL;
	if(device_node_lookup_lists[listPosition].length == 1) return device_node_lookup_lists[listPosition].storage[0];
	if(device_node_lookup_lists[listPosition].length > 1){
		for(int i=0; i<device_node_lookup_lists[listPosition].length; i++){
			if(strcmp(device_node_lookup_lists[listPosition].storage[i]->szDevicePath, pszDevicePathTmp)==0){
				return device_node_lookup_lists[listPosition].storage[i];
			}
		}
	}

	return NULL;	//	This should never reached
}

void device_node_lookup_lists_remove(DEVICE_NODE* pDeviceNodeTmp)
{
	uint16_t listPosition = 0;	//resDevTmp->resdevToken[0]<<8 | resDevTmp->resdevToken[1];

    //ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(device_node_lookup_lists[listPosition], pDeviceNodeTmp);
}


int registerDeviceNode(size_t pageTableAddress, int * pStatus, char* pszPath)
{
	DEVICE_NODE* pDeviceNodeTmp = (DEVICE_NODE*)kmalloc(sizeof(DEVICE_NODE), KMEM);

	strcpy(pDeviceNodeTmp->szDevicePath, pszPath);	//	[RESOURCE_DEVICE_NAME_LEN];

	device_node_lookup_lists_push(pDeviceNodeTmp);

	/*resDevTmp->resdevToken[0] 	= (iDevToken&0xFF00)>>8;	//	RESOURCE_DEVICE_TOKEN_LEN];
	resDevTmp->resdevToken[1] 	= (iDevToken&0xFF);

	resDevTmp->processDriver 	= lookupProcessWithPageTableAddress(pageTableAddress);;

	resDevTmp->piDevStatus 		= pStatus;

	resDevTmp->devopen 			= pFuncOpen;	//	pDevFunctions->devopen;
	resDevTmp->devread 			= pFuncRead;	//	pDevFunctions->devread;
	resDevTmp->devwrite 		= pFuncWrite;	//	pDevFunctions->devwrite;
	resDevTmp->devclose 		= pFuncClose;	//	pDevFunctions->devclose;

	resDevTmp->resdevIndex = resource_device_lookup_lists_push(resDevTmp);*/

	//	resDevTmp->resdevIndex = resource_device_lookup_lists_push(resDevTmp);	//	resdevToken
	return 1;//resDevTmp->resdevIndex;
}

size_t aether_queue_lookup_lists_getLength(uint32_t pid)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = pid&0xffff;
	return aether_queue_lookup_lists[listPosition].length;
}

size_t aether_queue_lookup_lists_push(uint32_t pid, AETHER_DESC* aetherTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = pid&0xffff;
    size_t idx = DYNARRAY_PUSHBACK(aether_queue_lookup_lists[listPosition], aetherTmp);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //	KDBG("The position for the %d aether_queue_lookup_lists:%d \n", listPosition, aether_queue_lookup_lists[listPosition].length);
    return idx;
}

size_t aether_queue_lookup_lists_connection_push(uint32_t pid, uint32_t chid, AETHER_CONNECTION_DESC* pConnTmp, THREAD_t* pThreadTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = pid&0xffff;
	if(aether_queue_lookup_lists[listPosition].length > chid){
	    size_t idxConnect = DYNARRAY_PUSHBACK(aether_queue_lookup_lists[listPosition].storage[chid]->pClientConnections, pConnTmp);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
	    //size_t idxThread  = DYNARRAY_PUSHBACK(aether_queue_lookup_lists[listPosition].storage[chid]->pThreadsConnections, pThreadTmp);
	    //if(idxConnect!=idxThread){
	    //	PANIC("Invalid status for aether_queue_lookup_lists.");
	    //}
	    return idxConnect;
	}
    //	KDBG("The position for the %d aether_queue_lookup_lists:%d \n", listPosition, aether_queue_lookup_lists[listPosition].length);
    return -1;
}

void aether_queue_lookup_lists_remove(uint32_t pid, AETHER_DESC* aetherTmp)
{
	uint16_t listPosition =	pid&0xffff;
    //ASSERT(prio < MAX_PRIORITYS);
	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aether_queue_lookup_lists[listPosition], aetherTmp);
}

void aether_queue_lookup_lists_connection_remove(uint32_t pid, uint32_t chid, AETHER_CONNECTION_DESC* aetherConnTmp)	//	, THREAD_t* pThreadTmp)
{
	//uint16_t listPosition =	pid&0xffff;
    //ASSERT(prio < MAX_PRIORITYS);

	AETHER_DESC* aetherTmp = aetherQueueLookupListsWithIndex(pid, chid);
	if(aetherTmp!=NULL){
		DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aetherTmp->pClientConnections, 	aetherConnTmp);
		//DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aether_queue_lookup_lists[listPosition].storage[chid]->pThreadsConnections, 	pThreadTmp);
	}
}

//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
AETHER_DESC* aetherQueueLookupListsWithIndex(uint32_t pid, int iAetherID)
{
	uint16_t listPosition = pid&0xffff;

	for(int i=0; i<aether_queue_lookup_lists[listPosition].length; i++){
		if(aether_queue_lookup_lists[listPosition].storage[i]->nChannelID==iAetherID){
			return aether_queue_lookup_lists[listPosition].storage[i];
		}
	}
	return NULL;

	//if(aether_queue_lookup_lists[listPosition].length > iAetherID){
	//	return aether_queue_lookup_lists[listPosition].storage[iAetherID];
	//}
	//return NULL;
}

size_t aether_msg_queue_pushPulse(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem){
	size_t idx = DYNARRAY_PUSHBACK(pAetherMsgQueue->pulseDataItems[pMsgItem->payload.pulse.nPulsePriority], pMsgItem);
	return idx;
}

size_t aether_msg_queue_pushMsg(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem){
	size_t idx = DYNARRAY_PUSHBACK(pAetherMsgQueue->msgDataItems, pMsgItem);
	return idx;
}

MSG_ITEM* aether_msg_queue_getPulse(AETHER_MSG_QUEUE* pAetherMsgQueue){
	for(int i=0; i<MAX_PRIORITYS; i++){
		if(pAetherMsgQueue->pulseDataItems[i].length>0){
			return pAetherMsgQueue->pulseDataItems[i].storage[0];
		}
	}
	return NULL;
}

MSG_ITEM* aether_msg_queue_getMsg(AETHER_MSG_QUEUE* pAetherMsgQueue){
	if(pAetherMsgQueue->msgDataItems.length>0){
		return pAetherMsgQueue->msgDataItems.storage[0];
	}

	return NULL;
}

void aether_msg_queue_removePulse(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem){
	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(pAetherMsgQueue->pulseDataItems[pMsgItem->payload.pulse.nPulsePriority], pMsgItem);
}

void aether_msg_queue_removeMsg(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem){
	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(pAetherMsgQueue->msgDataItems, pMsgItem);
}


size_t aether_connection_queue_lookup_lists_push(AETHER_CONNECTION_DESC* aetherConnTmp)
{
    size_t idx = DYNARRAY_PUSHBACK(aether_connection_queue_lookup_lists, aetherConnTmp);
    //	KDBG("The position for the %d aether_connection_queue_lookup_lists:%d \n", listPosition, aether_connection_queue_lookup_lists[listPosition].length);
    return idx;
}

void aether_connection_queue_lookup_lists_remove(AETHER_CONNECTION_DESC* aetherConnTmp)
{
	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(aether_connection_queue_lookup_lists, aetherConnTmp);
}

AETHER_CONNECTION_DESC* aetherConnectionQueueLookupListsWithIndex(int iAetherConnID)
{
	for(int i=0; i<aether_connection_queue_lookup_lists.length; i++){
		if(aether_connection_queue_lookup_lists.storage[i]->nConnectionID==iAetherConnID){
			return aether_connection_queue_lookup_lists.storage[i];
		}
	}
	return NULL;
}
//	AETHER_CONNECTION_DESC* aetherQueueLookupConnectionWithIndex(uint32_t pid, int iAetherID, int iConnID)
//	{
//		uint16_t listPosition = pid&0xffff;
//		if(aether_queue_lookup_lists[listPosition].length > iAetherID){
//			if(aether_queue_lookup_lists[listPosition].storage[iAetherID]->pClientConnections.length > iConnID){
//				return aether_queue_lookup_lists[listPosition].storage[iAetherID]->pClientConnections.storage[iConnID];
//			}
//		}
//		return NULL;
//	}

//	THREAD_t* aetherQueueLookupThreadWithIndex(uint32_t pid, int iAetherID, int iConnID)
//	{
//		uint16_t listPosition = pid&0xffff;
//		if(aether_queue_lookup_lists[listPosition].length > iAetherID){
//			if(aether_queue_lookup_lists[listPosition].storage[iAetherID]->pThreadsConnections.length > iConnID){
//				return aether_queue_lookup_lists[listPosition].storage[iAetherID]->pThreadsConnections.storage[iConnID];
//			}
//		}
//		return NULL;
//	}

size_t sleep_wait_wakeup_lookup_lists_push(SLEEP_THREAD_WAIT_WAKEUP* sleepWaitWakeup)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = 0;	//msgWaitReply->iDriverIndex&0xFFFF;//resDevTmp->resdevToken[1]<<16 | resDevTmp->resdevToken[0];	//(proc->arch.mmu.pml4&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(sleep_wait_queue_lookup_lists[listPosition], sleepWaitWakeup);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //	DBG("The position for the %d process_lookup_lists:%d \n", listPosition, file_descriptor_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
//SLEEP_THREAD_WAIT_WAKEUP* lookupSleepWaitReplyLookupListsWithThreadIndex(int iResDevIndex, int fd){	//	char* pszDevNameTmp){
//	uint16_t listPosition = iResDevIndex&0xFFFF;	//(devTokens&0xFFFF0000)>>16;
//	if(msg_wait_reply_lookup_lists[listPosition].length == 0) return NULL;
//	if(msg_wait_reply_lookup_lists[listPosition].length > 0) return msg_wait_reply_lookup_lists[listPosition].storage[0];
//	return NULL;	//	This should never reached
//}

THREAD_t* sleep_wait_wakeup_lookup_lists_update_remove()//SLEEP_THREAD_WAIT_WAKEUP* sleepWaitWakeup)
{
	uint16_t listPosition =	0;	//	msgWaitReply->iDriverIndex&0xFFFF;
	THREAD_t*  pThreadToRun = NULL;

    int iLastPositionSleeping = 0;	//	size_t iDynaRemovePackByValueTmp;
    for (iLastPositionSleeping = 0; iLastPositionSleeping < (size_t)(sleep_wait_queue_lookup_lists[listPosition]).length; ) {
        if ((sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->iSleepInterval <= 1 ) {
        	if(pThreadToRun==NULL){
            	(sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->pThread->state = THREAD_STATE_READY;
            	pThreadToRun = (sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->pThread;

                for (int jiDynaRemovePackByValueTmp = iLastPositionSleeping + 1; jiDynaRemovePackByValueTmp < (sleep_wait_queue_lookup_lists[listPosition]).length; jiDynaRemovePackByValueTmp++) {
                    (sleep_wait_queue_lookup_lists[listPosition]).storage[jiDynaRemovePackByValueTmp-1] = (sleep_wait_queue_lookup_lists[listPosition]).storage[jiDynaRemovePackByValueTmp];
                }
                --(sleep_wait_queue_lookup_lists[listPosition]).length;
        	}else{
            	(sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->iSleepInterval--;
            	iLastPositionSleeping++;
        	}
        }else{
        	(sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->iSleepInterval--;
        	iLastPositionSleeping++;
        }
    }

    return pThreadToRun;

    //	if (iDynaRemovePackByValueTmp == (size_t)((msg_wait_reply_lookup_lists[listPosition]).length))
    //    	iDynaRemovePackByValueTmp = -1;
    //	else
    //    --(msg_wait_reply_lookup_lists[listPosition]).length;
    //	iDynaRemovePackByValueTmp;

    //ASSERT(prio < MAX_PRIORITYS);
	//DYNARRAY_REMOVE_BY_VALUE_AND_PACK(msg_wait_reply_lookup_lists[listPosition], msgWaitReply);

}


void sleep_wait_wakeup_lookup_lists_update_only()//SLEEP_THREAD_WAIT_WAKEUP* sleepWaitWakeup)
{
	uint16_t listPosition =	0;	//	msgWaitReply->iDriverIndex&0xFFFF;

    int iLastPositionSleeping = 0;	//	size_t iDynaRemovePackByValueTmp;
    for (iLastPositionSleeping = 0; iLastPositionSleeping < (size_t)(sleep_wait_queue_lookup_lists[listPosition]).length; ) {
		(sleep_wait_queue_lookup_lists[listPosition]).storage[iLastPositionSleeping]->iSleepInterval--;
		iLastPositionSleeping++;
    }

    return ;
}

////////////////////////////////////////////////////////////////////////////////////
size_t interrupt_wait_queue_lookup_lists_push(int iIrq, THREAD_t* threadWaiting)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = iIrq;

    size_t idx = DYNARRAY_PUSHBACK(interrupt_wait_queue_lookup_lists[listPosition], threadWaiting);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    // KDBG("The position for the %d interrupt_wait_queue:%d \n", listPosition, interrupt_wait_queue_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
THREAD_t* lookupInterruptWaitQueueLookupListsWithIrq(int iIrq){	//	char* pszDevNameTmp){
	uint16_t listPosition = iIrq;
	if(interrupt_wait_queue_lookup_lists[listPosition].length == 0) return NULL;
	if(interrupt_wait_queue_lookup_lists[listPosition].length == 1) return interrupt_wait_queue_lookup_lists[listPosition].storage[0];
	if(interrupt_wait_queue_lookup_lists[listPosition].length > 1){
		return interrupt_wait_queue_lookup_lists[listPosition].storage[0];
	}


	return NULL;	//	This should never reached
}

void interrupt_wait_queue_lookup_lists_remove(int iIrq, THREAD_t* threadWaiting)
{
	uint16_t listPosition =	iIrq;

    //ASSERT(prio < MAX_PRIORITYS);
    //DYNARRAY_REMOVE_BY_VALUE_AND_PACK(interrupt_wait_queue_lookup_lists[listPosition], threadWaiting);

    size_t iDynaRemovePackByValueTmp;
    for (iDynaRemovePackByValueTmp = 0; iDynaRemovePackByValueTmp < (size_t)interrupt_wait_queue_lookup_lists[listPosition].length; iDynaRemovePackByValueTmp++) {
        if (	(interrupt_wait_queue_lookup_lists[listPosition].storage[iDynaRemovePackByValueTmp] == (threadWaiting))
        	|| (interrupt_wait_queue_lookup_lists[listPosition].storage[iDynaRemovePackByValueTmp] == (THREAD_t*)paddr2ptr((paddr_t)threadWaiting)) )
        {
            for (size_t jiDynaRemovePackByValueTmp = iDynaRemovePackByValueTmp + 1; jiDynaRemovePackByValueTmp < interrupt_wait_queue_lookup_lists[listPosition].length; jiDynaRemovePackByValueTmp++) {
            	interrupt_wait_queue_lookup_lists[listPosition].storage[jiDynaRemovePackByValueTmp-1] = interrupt_wait_queue_lookup_lists[listPosition].storage[jiDynaRemovePackByValueTmp];
            }
            interrupt_wait_queue_lookup_lists[listPosition].length--;
            //break;
        }
    }
    //if (iDynaRemovePackByValueTmp == (size_t)(interrupt_wait_queue_lookup_lists[listPosition].length))
    //    iDynaRemovePackByValueTmp = -1;
    //else
    //    --interrupt_wait_queue_lookup_lists[listPosition].length;
    //	iDynaRemovePackByValueTmp;
}


///////////////////////////////////////////////////////////////////////////////
size_t file_descriptor_lookup_lists_push(FILE_DESCRIPTOR* fileDescTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = fileDescTmp->processApplication->pid;//resDevTmp->resdevToken[1]<<16 | resDevTmp->resdevToken[0];	//(proc->arch.mmu.pml4&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(file_descriptor_lookup_lists[listPosition], fileDescTmp);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    // KDBG("The position for the %d process_lookup_lists:%d \n", listPosition, file_descriptor_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
FILE_DESCRIPTOR* lookupFileDescriptorLookupListsWithPidIndex(pid_t pid, int fd){	//	char* pszDevNameTmp){
	uint16_t listPosition = pid&0xFFFF;	//(devTokens&0xFFFF0000)>>16;
	if(file_descriptor_lookup_lists[listPosition].length == 0) return NULL;
	if(file_descriptor_lookup_lists[listPosition].length == 1) return file_descriptor_lookup_lists[listPosition].storage[0];
	if(file_descriptor_lookup_lists[listPosition].length > fd){
		return file_descriptor_lookup_lists[listPosition].storage[fd];
	}


	return NULL;	//	This should never reached
}

void file_descriptor_lookup_lists_remove(FILE_DESCRIPTOR* fileDescTmp)
{
	uint16_t listPosition =	fileDescTmp->processApplication->pid;

    //ASSERT(prio < MAX_PRIORITYS);
	if(file_descriptor_lookup_lists[listPosition].length > fileDescTmp->fd){
		file_descriptor_lookup_lists[listPosition].storage[fileDescTmp->fd]->iStatus=-1; //	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(resource_device_lookup_lists[listPosition], fileDescTmp);
	}
}



size_t msg_wait_reply_lookup_lists_push(MSG_WAIT_REPLY* msgWaitReply)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = msgWaitReply->iDriverIndex&0xFFFF;//resDevTmp->resdevToken[1]<<16 | resDevTmp->resdevToken[0];	//(proc->arch.mmu.pml4&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(msg_wait_reply_lookup_lists[listPosition], msgWaitReply);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    //	DBG("The position for the %d process_lookup_lists:%d \n", listPosition, file_descriptor_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
MSG_WAIT_REPLY* lookupMsgWaitReplyLookupListsWithResDevIndex(int iResDevIndex, int fd){	//	char* pszDevNameTmp){
	uint16_t listPosition = iResDevIndex&0xFFFF;	//(devTokens&0xFFFF0000)>>16;
	if(msg_wait_reply_lookup_lists[listPosition].length == 0) return NULL;
	if(msg_wait_reply_lookup_lists[listPosition].length > 0) return msg_wait_reply_lookup_lists[listPosition].storage[0];
	//if(msg_wait_reply_lookup_lists[listPosition].length > fd){
	//	return msg_wait_reply_lookup_lists[listPosition].storage[fd];
	//}


	return NULL;	//	This should never reached
}

void msg_wait_reply_lookup_lists_remove(MSG_WAIT_REPLY* msgWaitReply)
{
	uint16_t listPosition =	msgWaitReply->iDriverIndex&0xFFFF;

    //ASSERT(prio < MAX_PRIORITYS);

	DYNARRAY_REMOVE_BY_VALUE_AND_PACK(msg_wait_reply_lookup_lists[listPosition], msgWaitReply);

}



size_t resource_device_lookup_lists_push(RESOURCE_DEVICE* resDevTmp)	//	char resdevToken[RESOURCE_DEVICE_TOKEN_LEN]
{
	uint16_t listPosition = 0;	//resDevTmp->resdevToken[0]<<8 | resDevTmp->resdevToken[1];	//(proc->arch.mmu.pml4&0xFFFF0000)>>16;

    //ASSERT(prio < MAX_PRIORITYS);

    size_t idx = DYNARRAY_PUSHBACK(resource_device_lookup_lists[listPosition], resDevTmp);	//	DYNARRAY_INSERT(thread_runing_lists[prio], thread);
    // KDBG("The position for the %d process_lookup_lists:%d \n", listPosition, resource_device_lookup_lists[listPosition].length);
    return idx;
}


//	For example, the token for serial device is: "sr", the value of the toke should be 's'<<16 | 'r'
RESOURCE_DEVICE* lookupResourceDeviceLookupListsWithDevName(size_t devTokens, const char* pszDevNameTmp){
	uint16_t listPosition = 0;	//devTokens;	//(devTokens&0xFFFF0000)>>16;
	if(resource_device_lookup_lists[listPosition].length == 0) return NULL;
	if(resource_device_lookup_lists[listPosition].length == 1) return resource_device_lookup_lists[listPosition].storage[0];
	if(resource_device_lookup_lists[listPosition].length > 1){
		for(int i=0; i<resource_device_lookup_lists[listPosition].length; i++){
			if(strcmp(resource_device_lookup_lists[listPosition].storage[i]->szDevName, pszDevNameTmp)==0){
				return resource_device_lookup_lists[listPosition].storage[i];
			}
		}
	}

	return NULL;	//	This should never reached
}

void resource_device_lookup_lists_remove(RESOURCE_DEVICE* resDevTmp)
{
	uint16_t listPosition = 0;	//resDevTmp->resdevToken[0]<<8 | resDevTmp->resdevToken[1];

    //ASSERT(prio < MAX_PRIORITYS);

    DYNARRAY_REMOVE_BY_VALUE_AND_PACK(resource_device_lookup_lists[listPosition], resDevTmp);
}


int registerDevice(size_t pageTableAddress, int * pStatus, char* pszPath, int  	iDevToken, void*   pFuncOpen, void*   pFuncRead, void*   pFuncWrite, void*   pFuncClose)
{
	RESOURCE_DEVICE* resDevTmp = (RESOURCE_DEVICE*)kmalloc(sizeof(RESOURCE_DEVICE), KMEM);

	strcpy(resDevTmp->szDevName, pszPath);	//	[RESOURCE_DEVICE_NAME_LEN];

	resDevTmp->resdevToken[0] 	= (iDevToken&0xFF00)>>8;	//	RESOURCE_DEVICE_TOKEN_LEN];
	resDevTmp->resdevToken[1] 	= (iDevToken&0xFF);

	resDevTmp->processDriver 	= lookupProcessWithPageTableAddress(pageTableAddress);;

	resDevTmp->piDevStatus 		= pStatus;

	resDevTmp->devopen 			= pFuncOpen;	//	pDevFunctions->devopen;
	resDevTmp->devread 			= pFuncRead;	//	pDevFunctions->devread;
	resDevTmp->devwrite 		= pFuncWrite;	//	pDevFunctions->devwrite;
	resDevTmp->devclose 		= pFuncClose;	//	pDevFunctions->devclose;

	resDevTmp->resdevIndex = resource_device_lookup_lists_push(resDevTmp);

	//	resDevTmp->resdevIndex = resource_device_lookup_lists_push(resDevTmp);	//	resdevToken
	return resDevTmp->resdevIndex;
}

int msg_sys_open(size_t pageTableAddress, const char *pszPath, const char *pszMode){
	//	const char* pszPathOrg =  pszPath;
	//	const char* pszModeOrg =  pszMode;
	int 	(*devopenTmp)	();	//char *pathname, char *mode);

	int  ret = -1;
	//	pszPath = "/dev/ser1";

	devopenTmp = NULL;


	PROCESS_t* processApplication = lookupProcessWithPageTableAddress(pageTableAddress);

	size_t devTokens = 's' << 8 | 'r';	// /dev/ser
	if(strlen(pszPath)>=8){
		if((pszPath[5]=='k')&&(pszPath[6]=='b')&&(pszPath[7]=='d')){	//	/dev/kbd
			devTokens = 'k' << 8 | 'b';
		}
	}

	RESOURCE_DEVICE* resDevTmp = lookupResourceDeviceLookupListsWithDevName(devTokens, pszPath);

	if(resDevTmp!=NULL){
		FILE_DESCRIPTOR* fileDescriptor 	= (FILE_DESCRIPTOR*)kmallocz(sizeof(FILE_DESCRIPTOR));
		fileDescriptor->dev_entry 			= resDevTmp;
		fileDescriptor->dev_lock			= NULL;
		fileDescriptor->processApplication 	= processApplication;
		fileDescriptor->iStatus				= 0;	//	NULL;	//	fileDescriptor->pStatus				= NULL;
		fileDescriptor->fd					= file_descriptor_lookup_lists_push(fileDescriptor);

		if((fileDescriptor->dev_entry!=NULL)&&(fileDescriptor->dev_entry->devopen!=NULL)){
			devopenTmp =fileDescriptor->dev_entry->devopen;
			if(devopenTmp){
				//	devopenTmp();//pszPathOrg, pszModeOrg);		//"/dev/ser1", "rw");//(*devopenTmp)(pszPathOrg, pszModeOrg);	//	ret = (*devopenTmp)(pszPathOrg, pszModeOrg);
			}
		}

		ret = fileDescriptor->fd;
	}

	//arch_pte_restore(pageTableAddress);

	return	ret;	//	fileDescriptor->fd;	//	

	//if((resDevTmp!=NULL)&&(resDevTmp->devopen!=NULL)&&(resDevTmp->devopen(pathname, mode)<0))return -1;
	//else return	fileDescriptor->fd;
	//	if(stream)stream->resource_device_item->devfopen(stream);
}

//#define CURRENT_THREAD  (get_cpuvar()->current_task)
size_t msg_sys_read(size_t pageTableAddress, int fd, MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen){		//	void *buff, size_t nbytes) {
	int 		ret	= 0;

	PROCESS_t* processApplication = lookupProcessWithPageTableAddress(pageTableAddress);

	FILE_DESCRIPTOR* fdTmp = lookupFileDescriptorLookupListsWithPidIndex(processApplication->pid, fd);
	if(fdTmp==NULL) ret = 0;


	CURRENT_THREAD->state = THREAD_STATE_SIGWAITINFO;


	MSG_WAIT_REPLY* pMsgWaitReply = (MSG_WAIT_REPLY*)kmallocz(sizeof(MSG_WAIT_REPLY));
	pMsgWaitReply->pThread = CURRENT_THREAD;
	pMsgWaitReply->iDriverIndex = fdTmp->dev_entry->resdevIndex;;
	pMsgWaitReply->fdApplication = fd;

	pMsgWaitReply->pMsgSend  = (MSG_DATA*)map_user_data((void*)pMsgSend, sizeof(MSG_DATA));
	pMsgWaitReply->pMsgReply = (MSG_DATA*)map_user_data((void*)pMsgReply, sizeof(MSG_DATA));

	//	pMsgWaitReply->pMsgSend  = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgSend);
	//	pMsgWaitReply->pMsgReply = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgReply);

	pMsgWaitReply->iMsgType = MSG_READ;

	pMsgWaitReply->iMsgStatus 	= MSG_STATUS_INIT;

	//pMsgWaitReply->pszMsgSend 	= arch_vm_v2p_by_pte(pageTableAddress, pMsgSend->pszMsg);
	//pMsgWaitReply->iMsgSendLen 	= iSendLen;

	//pMsgWaitReply->pszMsgReply 	= arch_vm_v2p_by_pte(pageTableAddress, pMsgReply->pszMsg);
	
	pMsgWaitReply->piMsgReplyLen	= (int*)map_user_data(piReplyLen, sizeof(int*));
	//pMsgWaitReply->piMsgReplyLen	= (int*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)piReplyLen);	//pMsgWaitReply->piMsgReplyLen= arch_vm_v2p_by_pte(pageTableAddress, piReplyLen);
	msg_wait_reply_lookup_lists_push(pMsgWaitReply);

	if((fdTmp->dev_entry==NULL)||(fdTmp->dev_entry->devclose==NULL)){
		ret = 0;
	} else {
		ret = fdTmp->dev_entry->devread();//fd, NULL, 0);	//pMsgWaitReply->pszMsgReply, *(pMsgWaitReply->piMsgReplyLen) );
	}

	//	arch_pte_restore(pageTableAddress);	//	EARLY_PML4_BASE);	//	__kernel_pml4);

	return ret;
	//	if(stream)stream->resource_device_item->devfread(stream);
}

size_t msg_sys_write(size_t pageTableAddress, int fd, MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen){		//	 const void *buff, size_t nbytes){
	int 		iRslt	= 0;

	PROCESS_t* processApplication = lookupProcessWithPageTableAddress(pageTableAddress);

	FILE_DESCRIPTOR* fdTmp = lookupFileDescriptorLookupListsWithPidIndex(processApplication->pid, fd);
	if(fdTmp==NULL) iRslt = 0;

	MSG_WAIT_REPLY* pMsgWaitReply = (MSG_WAIT_REPLY*)kmallocz(sizeof(MSG_WAIT_REPLY));
	pMsgWaitReply->pThread = CURRENT_THREAD;
	pMsgWaitReply->pThread->state = THREAD_STATE_SIGWAITINFO;

	pMsgWaitReply->iDriverIndex = (fdTmp->dev_entry==NULL)?0:(fdTmp->dev_entry->resdevIndex);
	pMsgWaitReply->fdApplication = fd;

	pMsgWaitReply->pMsgSend = (MSG_DATA*)map_user_data((void*)pMsgSend, sizeof(MSG_DATA));
	//pMsgWaitReply->pMsgSend = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgSend);
	//pMsgWaitReply->pMsgSend->pszMsg = arch_vm_v2p_by_pte(pageTableAddress, pMsgWaitReply->pMsgSend->pszMsg);

	pMsgWaitReply->pMsgReply = (MSG_DATA*)map_user_data((void*)pMsgReply, sizeof(MSG_DATA));
	//pMsgWaitReply->pMsgReply = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgReply);
	//pMsgWaitReply->pMsgReply->pszMsg = arch_vm_v2p_by_pte(pageTableAddress, pMsgWaitReply->pMsgReply->pszMsg);

	pMsgWaitReply->iMsgType 	= MSG_WRITE;

	pMsgWaitReply->iMsgStatus 	= MSG_STATUS_INIT;

	//pMsgWaitReply->pszMsgSend 	= arch_vm_v2p_by_pte(pageTableAddress, pMsgSend->pszMsg);
	//pMsgWaitReply->iMsgSendLen 	= iSendLen;

	//pMsgWaitReply->pszMsgReply 	= arch_vm_v2p_by_pte(pageTableAddress, pMsgReply->pszMsg);
	
	pMsgWaitReply->piMsgReplyLen	= (int*)map_user_data((void*)piReplyLen, sizeof(int*));
	//pMsgWaitReply->piMsgReplyLen	= (int*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)piReplyLen);
	msg_wait_reply_lookup_lists_push(pMsgWaitReply);

	if((fdTmp->dev_entry==NULL)||(fdTmp->dev_entry->devclose==NULL)){
		iRslt = 0;
	} else {
		iRslt = fdTmp->dev_entry->devwrite();//fd, NULL, 0);	//	buff, nbytes);
	}

	//arch_pte_restore(pageTableAddress);	//	EARLY_PML4_BASE);	//	__kernel_pml4);

	return iRslt;
}

size_t 	msg_sys_recv(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgSend, int iSendLen, MSG_CONTEXT* pMsgContext){
	//void* pszSendTmp = pMsgSend->pszMsg;
	int 		ret	= -1;

	MSG_WAIT_REPLY* msgWaitReplyTmp = lookupMsgWaitReplyLookupListsWithResDevIndex(iDriverIndex, 0);

	if(msgWaitReplyTmp!=NULL){

		MSG_DATA* pMsgSendTmp = (MSG_DATA*)map_user_data((void*)pMsgSend, sizeof(MSG_DATA));
		//MSG_DATA* pMsgSendTmp = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgSend);	// char* pszMsg	->pszMsg
		char* pszMsgTmp = pMsgSendTmp->szMsg;	//	arch_vm_v2p_by_pte(pageTableAddress, pszSendTmp);//pMsgSendTmp->pszMsg);
		char* pszMsgSend =  msgWaitReplyTmp->pMsgSend->szMsg;
		while((*pszMsgSend)!='\0'){
			*pszMsgTmp++ = *pszMsgSend++;
		}
		*pszMsgTmp = '\0';
		//strcpy(pszMsgTmp, msgWaitReplyTmp->pMsgSend->pszMsg);	//	arch_vm_v2p_by_pte(pageTableAddress, msgWaitReplyTmp->pMsgSend->pszMsg) );
		pMsgSendTmp->iMsgType = msgWaitReplyTmp->pMsgSend->iMsgType;
		pMsgSendTmp->iMsgLen  = msgWaitReplyTmp->pMsgSend->iMsgLen;

		ret = iSendLen;

		msgWaitReplyTmp->iMsgStatus 	= MSG_STATUS_RECV;

		unmap_user_data((void*)pMsgSendTmp, sizeof(MSG_DATA));
		// KDBG("Msg Received: %d [%s]\n", pMsgSendTmp->szMsg[0], pMsgSendTmp->szMsg+1);
	}else{
		//	DBG("Msg Not Received \n");
		ret = -1;
	}

	//	arch_pte_restore(pageTableAddress);

	return ret;
}

size_t 	msg_sys_reply(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgReply, int iReplyLen, MSG_CONTEXT* pMsgContext){
	//	int				iMsgReplyType			= pMsgReply->iMsgType;
	int 		ret	= 0;


	MSG_WAIT_REPLY* msgWaitReplyTmp = lookupMsgWaitReplyLookupListsWithResDevIndex(iDriverIndex, 0);

	if(msgWaitReplyTmp!=NULL){//	return -1;
		if(msgWaitReplyTmp->iMsgStatus 	== MSG_STATUS_RECV){


			MSG_DATA* pMsgReplyTmp = (MSG_DATA*)map_user_data((void*)pMsgReply, sizeof(MSG_DATA));
			//	MSG_DATA* pMsgReplyTmp = (MSG_DATA*)arch_vm_v2p_by_pte(pageTableAddress, (vaddr_t)pMsgReply);	// char* pszMsg	->pszMsg

			// KDBG("Msg Replied : %d [%s]\n", pMsgReplyTmp->szMsg[0], pMsgReplyTmp->szMsg);

			// KDBG("Msg Replied copy to buffer: [%s]\n", msgWaitReplyTmp->pMsgReply->szMsg);

			char* pszMsgTmp = pMsgReplyTmp->szMsg;	//	arch_vm_v2p_by_pte(pageTableAddress, pszSendTmp);//pMsgSendTmp->pszMsg);
			char* pszMsgSend =  msgWaitReplyTmp->pMsgReply->szMsg;
			/*pszMsgSend[1] = 't';
			pszMsgSend[2] = 't';
			pszMsgSend[3] = 't';
			pszMsgSend[4] = 't';
			pszMsgSend[5] = 't';*/
			while((*pszMsgTmp)!='\0'){
				*pszMsgSend++ = *pszMsgTmp++;
			}
			*pszMsgSend = '\0';

			msgWaitReplyTmp->pMsgReply->iMsgLen			= pMsgReplyTmp->iMsgLen;
			msgWaitReplyTmp->pMsgReply->iMsgType		= pMsgReplyTmp->iMsgType;

			*(msgWaitReplyTmp->piMsgReplyLen) 			= sizeof(MSG_DATA);
			msgWaitReplyTmp->pThread->state 			= THREAD_STATE_READY;

			msg_wait_reply_lookup_lists_remove(msgWaitReplyTmp);

			unmap_user_data((void*)pMsgReplyTmp, sizeof(MSG_DATA));

			unmap_user_data(msgWaitReplyTmp->pMsgSend, sizeof(MSG_DATA));
			unmap_user_data(msgWaitReplyTmp->pMsgReply, sizeof(MSG_DATA));
			unmap_user_data(msgWaitReplyTmp->piMsgReplyLen, sizeof(int*));
			kfree(msgWaitReplyTmp);
			ret = sizeof(MSG_DATA);
		}else{
			ret = -1;
		}
	}else{
		ret = -1;
	}
	//	char* pszMsg = arch_vm_v2p_by_pte(pageTableAddress, pMsgSend->pszMsg);
	//	strcpy(pszMsg, msgWaitReplyTmp->pszMsgSend);

	//arch_pte_restore(pageTableAddress);

	// KDBG("Msg Replied");

	return ret;	//	sizeof(MSG_DATA);
}

int msg_sys_close(size_t pageTableAddress, int fd){
	int 		ret	= 0;
	PROCESS_t* processApplication = lookupProcessWithPageTableAddress(pageTableAddress);

	//	The file descriptor will not be removed and there should be memory leak
	FILE_DESCRIPTOR* fdTmp = lookupFileDescriptorLookupListsWithPidIndex(processApplication->pid, fd);
	file_descriptor_lookup_lists_remove(fdTmp);

	if(fdTmp==NULL) ret = 0;

	if((fdTmp->dev_entry==NULL)||(fdTmp->dev_entry->devclose==NULL)){
		ret = 0;
	} else {
		ret = fdTmp->dev_entry->devclose();//fd);
	}


	//	arch_pte_restore(pageTableAddress);	//	EARLY_PML4_BASE);	//	__kernel_pml4);

	// KDBG("The file handler closed. \n");

	return ret;
}

/*
FILE* fopen(const char *pathname, const char *mode){
	RESOURCE_DEVICE* resDevTmp = lookupResourceDeviceLookupListsWithDevName("sr", pathname);
	FILE*
	return
	//	if(stream)stream->resource_device_item->devfopen(stream);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
	if(stream)stream->resource_device_item->devfread(stream);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,  FILE *stream){
	if(stream)stream->resource_device_item->devfwrite(stream);
}

int fclose(FILE *stream){
	if(stream)stream->resource_device_item->devfclose(stream);
}
 */
