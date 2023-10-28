/*
 * ky_resmgr.h
 *
 *  Created on: 2022-7-25
 *      Author: Test
 */

#ifndef OS_KERNEL_RESMGR_KY_RESMGR_H_
#define OS_KERNEL_RESMGR_KY_RESMGR_H_

#include "task.h"
#include "locks.h"
#include "ker_time.h"
//#include "type.h"
//#include "public/kernel/types.h"
//#include "public/kernel/objects.h"
//#include "data_types.h"


#define DEVICE_NODE_PATH_LEN_MAX	1024

#define RESOURCE_DEVICE_NAME_LEN	1024
#define RESOURCE_DEVICE_TOKEN_LEN 	2


#define MSG_STATUS_INIT 	0
#define MSG_STATUS_RECV 	1
#define MSG_STATUS_REPLY 	2


typedef struct device_node_entry{
	char 		szDevicePath[DEVICE_NODE_PATH_LEN_MAX];
	pid_t		iPid;
	uint32_t	nChannelID;
}DEVICE_NODE;

////////////////////////////////////////////////////////////////////////

typedef struct resource_device_entry{
	char 	szDevName[RESOURCE_DEVICE_NAME_LEN];
	char 	resdevToken[RESOURCE_DEVICE_TOKEN_LEN];
	int  	resdevIndex;
	int*	piDevStatus;
    PROCESS_t* processDriver;


    int 	(*devopen)	();//const char *pathname, const char *mode);
    size_t 	(*devread)	();//int fd, void *buff, size_t nbytes);
    size_t 	(*devwrite)	();//int fd, const void *buff, size_t nbytes);
    int 	(*devclose)	();//int fd);

	//FILE* (*devfopen)(const char *pathname, const char *mode);
    //size_t (*devfread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
    //size_t (*devfwrite)(const void *ptr, size_t size, size_t nmemb,  FILE *stream);
    //int (*devfclose)(FILE *stream);
}RESOURCE_DEVICE;

/*
struct _io_object {
	RESOURCE_DEVICE* resource_device_item;	// The resource device item
	int		iType;							// 0: Normal file; 1: Device
	char   *pInputPos; 						// The next input position for the file
	char   *pStartPos; 						// The base position for the file, the start position of the file
	int 	iFileFlags; 					// The file tokens
	int 	iFileSize; 						// The file size
	char   *pszFileName; 					// The temporary file name
};
typedef struct _io_object FILE;	*/

typedef struct file_descriptor_entry{
	int					fd;	//file_descriptor_index;
	RESOURCE_DEVICE* 	dev_entry;
	PROCESS_t* 			processApplication;
	struct spinlock*	dev_lock;
	int					iStatus;	//	int*				pStatus;
}FILE_DESCRIPTOR;



typedef struct _msg_wait_reply{
	THREAD_t* pThread;

	MSG_DATA* pMsgSend;
	MSG_DATA* pMsgReply;

	int		iDriverIndex;
	int		fdApplication;

	int 	iMsgType;

	int 	iMsgStatus;
	//	int		iMsgSendLen;
	//	char* 	pszMsgSend;
	//	char*   pszMsgReply;
	int*    piMsgReplyLen;
}MSG_WAIT_REPLY;


typedef struct _sleep_thread_wait_wakeup{
	THREAD_t* pThread;
	int		iSleepInterval;
}SLEEP_THREAD_WAIT_WAKEUP;

typedef struct _msg_item
{
	uint32_t	nChannelID;		//
	uint32_t	nConnectionID;	//	The connection id also used as receive id
	THREAD_t	*pSendBlockThread;
	union{
		struct{
			uint32_t nPulseCode;
			uint32_t nPulseValue;
			uint32_t nPulsePriority;
		}pulse;
		struct{
			iov_t* siov;	//	const iov_t* siov;
			int sparts;
			iov_t* riov;	//	const iov_t* riov;
			int rparts;
			iov_t* riov_org;
		}msg;
	}payload;
} MSG_ITEM;

typedef struct _aether_msg_queue
{
	DYNARRAY_STRUCT(MSG_ITEM *) 	msgDataItems;						//  Packages for the connections
	DYNARRAY_STRUCT(MSG_ITEM *) 	pulseDataItems[MAX_PRIORITYS];		//  Packages for the connections
} AETHER_MSG_QUEUE;

typedef struct _msg_recv_block_desc
{
	//uint32_t	nChannelID;		//
	//uint32_t	nConnectionID;	//	The connection id also used as receive id
	struct{
		iov_t* siov;	//	const iov_t* siov;
		int sparts;
		iov_t* riov;	//	const iov_t* riov;
		int rparts;
		iov_t* riov_org;
	}msg;
	THREAD_t*  	pThreadSendRecvBlocked;
} MSG_SENDRECV_BLOCK_DESC;

typedef struct _pulse_recv_block_desc
{
	//uint32_t	nChannelID;		//
	//uint32_t	nConnectionID;	//	The connection id also used as receive id
	struct _pulse * pPulse;
	THREAD_t*  		pThreadRecvPulseBlocked;
} PULSE_RECV_BLOCK_DESC;

struct _aether_desc;
typedef struct _aether_connection_desc
{
	size_t							nChannelID;							//	Channels
	uint32_t						nConnectionID;						//	Connections
	pid_t							pidConn;							//	Connections
	uint32_t						fValid;								//	Whether the connection still valid
	THREAD_t *						pThreadRelated;						//	The thread set up the connection
	MSG_SENDRECV_BLOCK_DESC			msgSendBlockedThreadDesc;
	struct _aether_desc*			pAetherDesc;						//	The aether related
} AETHER_CONNECTION_DESC;


typedef struct _aether_desc
{
	uint32_t									nPID;									//
	uint32_t									nChannelID;								//
	uint32_t									nConnectionCount;						//	Connections
	uint32_t									fValid;									//  Whether the channel still valid
	uint32_t									nflags;
	uint32_t									nReserve;
	FILE_DESCRIPTOR*							pFileWithBus[MAX_THREAD_PER_PROC];		//	File Descriptor for channel
	THREAD_t *									pThreadAether;							//	DYNARRAY_STRUCT(THREAD_t *)					pThreadsConnections;					//  Threads for the connections
	DYNARRAY_STRUCT(AETHER_CONNECTION_DESC *)	pClientConnections;						//  Connection informations for the connections
	AETHER_MSG_QUEUE							msgQueue;								//	Message Queue for the channel
	DYNARRAY_STRUCT(MSG_SENDRECV_BLOCK_DESC *)			recvMsgBlockThreadsQueue;				//	Receive message thread list
	DYNARRAY_STRUCT(PULSE_RECV_BLOCK_DESC *)			recvPulseBlockThreadsQueue;				//	Receive pulse thread list
} AETHER_DESC;



void resmgr_init();
uint32_t resmgr_get_unique_random_int(int fPosValue);

size_t 	timer_event_queue_lookup_lists_push(Timer* pSigEvent);
Timer* lookupTimerEventQueueLookupListsWithTimerID(int iTimerID);
int 	getTimerEventQueueLookupListsLength();
void 	timer_event_queue_lookup_lists_remove(Timer* pSigEvent);

size_t 	interrupt_event_lookup_lists_push(int iIrq, struct sigevent * pSigEventTmp);
struct sigevent * lookupInterruptEventLookupListsWithIrq(int iIrq);
int 	getLookupInterruptEventLookupListsLength(int iIrq);
struct sigevent * lookupInterruptEventLookupListsWithIrqIndex(int iIrq, int iIndex);
void 	interrupt_event_lookup_lists_remove(int iIrq, struct sigevent * pSigEventTmp);

size_t 	receive_msg_block_queue_lookup_lists_push(AETHER_DESC* aetherDescTmp, MSG_SENDRECV_BLOCK_DESC* threadRecvMsgBlocking);
MSG_SENDRECV_BLOCK_DESC* lookupReceiveMsgBlockQueueLookupLists(AETHER_DESC* aetherDescTmp);
void 	receive_msg_block_queue_lookup_lists_remove(AETHER_DESC* aetherDescTmp, MSG_SENDRECV_BLOCK_DESC* threadRecvMsgBlocking);

size_t 	receive_pulse_block_queue_lookup_lists_push(AETHER_DESC* aetherDescTmp, PULSE_RECV_BLOCK_DESC* threadRecvPulseBlocking);
PULSE_RECV_BLOCK_DESC* lookupReceivePulseBlockQueueLookupLists(AETHER_DESC* aetherDescTmp);
void 	receive_pulse_block_queue_lookup_lists_remove(AETHER_DESC* aetherDescTmp, PULSE_RECV_BLOCK_DESC* threadRecvPulseBlocking);

size_t 	device_node_lookup_lists_push(DEVICE_NODE* pDeviceNodeTmp);
DEVICE_NODE* lookupDeviceNodeLookupListsWithDevicePath(const char* pszDevicePathTmp);
void 	device_node_lookup_lists_remove(DEVICE_NODE* pDeviceNodeTmp);
int 	registerDeviceNode(size_t pageTableAddress, int * pStatus, char* pszPath);

size_t 	aether_queue_lookup_lists_getLength(uint32_t pid);
size_t 	aether_queue_lookup_lists_push(uint32_t pid, AETHER_DESC* aetherTmp);
size_t 	aether_queue_lookup_lists_connection_push(uint32_t pid, uint32_t chid, AETHER_CONNECTION_DESC* pConnTmp, THREAD_t* pThreadTmp);
void 	aether_queue_lookup_lists_remove(uint32_t pid, AETHER_DESC* aetherTmp);
void 	aether_queue_lookup_lists_connection_remove(uint32_t pid, uint32_t chid, AETHER_CONNECTION_DESC* aetherConnTmp);	//, THREAD_t* pThreadTmp);
AETHER_DESC* aetherQueueLookupListsWithIndex(uint32_t pid, int iAetherID);
//AETHER_CONNECTION_DESC* aetherQueueLookupConnectionWithIndex(uint32_t pid, int iAetherID, int iConnID);
//THREAD_t* aetherQueueLookupThreadWithIndex(uint32_t pid, int iAetherID, int iConnID);

size_t 	aether_connection_queue_lookup_lists_push(AETHER_CONNECTION_DESC* aetherConnTmp);
void 	aether_connection_queue_lookup_lists_remove(AETHER_CONNECTION_DESC* aetherConnTmp);
AETHER_CONNECTION_DESC* aetherConnectionQueueLookupListsWithIndex(int iAetherConnID);

size_t 	aether_msg_queue_pushPulse(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem);
size_t 	aether_msg_queue_pushMsg(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem);
MSG_ITEM* aether_msg_queue_getPulse(AETHER_MSG_QUEUE* pAetherMsgQueue);
MSG_ITEM* aether_msg_queue_getMsg(AETHER_MSG_QUEUE* pAetherMsgQueue);
void 	aether_msg_queue_removePulse(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem);
void 	aether_msg_queue_removeMsg(AETHER_MSG_QUEUE* pAetherMsgQueue, MSG_ITEM* pMsgItem);

size_t 	sleep_wait_wakeup_lookup_lists_push(SLEEP_THREAD_WAIT_WAKEUP* sleepWaitWakeup);
THREAD_t* sleep_wait_wakeup_lookup_lists_update_remove();
void sleep_wait_wakeup_lookup_lists_update_only();

size_t 	interrupt_wait_queue_lookup_lists_push(int iIrq, THREAD_t* threadWaiting);
THREAD_t* lookupInterruptWaitQueueLookupListsWithIrq(int iIrq);
void 	interrupt_wait_queue_lookup_lists_remove(int iIrq, THREAD_t* threadWaiting);

size_t 	file_descriptor_lookup_lists_push(FILE_DESCRIPTOR* fileDescTmp);
FILE_DESCRIPTOR* lookupFileDescriptorLookupListsWithPidIndex(pid_t pid, int fd);
void 	file_descriptor_lookup_lists_remove(FILE_DESCRIPTOR* fileDescTmp);

size_t 	msg_wait_reply_lookup_lists_push(MSG_WAIT_REPLY* msgWaitReply);
MSG_WAIT_REPLY* lookupMsgWaitReplyLookupListsWithResDevIndex(int iResDevIndex, int fd);
void 	msg_wait_reply_lookup_lists_remove(MSG_WAIT_REPLY* msgWaitReply);

size_t 	resource_device_lookup_lists_push(RESOURCE_DEVICE* resDevTmp);
RESOURCE_DEVICE* lookupResourceDeviceLookupListsWithDevName(size_t devTokens, const char* pszDevNameTmp);
void 	resource_device_lookup_lists_remove(RESOURCE_DEVICE* resDevTmp);


int 	registerDevice(size_t pageTableAddress, int * pStatus, char* pszPath, int  	iDevToken, void*   pFuncOpen, void*   pFuncRead, void*   pFuncWrite, void*   pFuncClose);
int 	msg_sys_open(size_t pageTableAddress, const char *pszPath, const char *pszMode);
size_t 	msg_sys_read(size_t pageTableAddress, int fd, MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen);
size_t 	msg_sys_write(size_t pageTableAddress, int fd, MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen);
size_t 	msg_sys_recv(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgSend, int iSendLen, MSG_CONTEXT* pMsgContext);
size_t 	msg_sys_reply(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgReply, int iReplyLen, MSG_CONTEXT* pMsgContext);
int 	msg_sys_close(size_t pageTableAddress, int fd);



/*
//	return value:  resdevIndex or the index for the resouce_device instance
int 	registerDevice(size_t pageTableAddress, int * pStatus, char* pszPath, int  	iDevToken, void*   pFuncOpen, void*   pFuncRead, void*   pFuncWrite, void*   pFuncClose);
//int registerDevice(char resdevToken[RESOURCE_DEVICE_TOKEN_LEN], RESOURCE_DEVICE* resDevTmp);

int 	msg_sys_open(size_t pageTableAddress, const char *pszPath, const char *pszMode);	//	FILE_DESCRIPTOR* 	ky_sys_open(PROCESS* processApplication, const char *pathname, const char *mode);
size_t 	msg_sys_read(size_t pageTableAddress, int fd, MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen);		//	 void *buff, size_t nbytes);
size_t 	msg_sys_write(size_t pageTableAddress, int fd,  MSG_DATA* pMsgSend, int iSendLen, MSG_DATA* pMsgReply, int* piReplyLen);	//	const void *buff, size_t nbytes);

size_t 	msg_msg_recv(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgSend, int iSendLen, MSG_CONTEXT* pMsgContext);
size_t 	msg_msg_reply(size_t pageTableAddress, int iDriverIndex, MSG_DATA* pMsgReply, int iReplyLen, MSG_CONTEXT* pMsgContext);

int 	msg_sys_close(size_t pageTableAddress, int fd);
*/
//	FILE* fopen(const char *pathname, const char *mode);
//	size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
//	size_t fwrite(const void *ptr, size_t size, size_t nmemb,  FILE *stream);
//	int fclose(FILE *stream);

THREAD_t* lookupInterruptWaitQueueLookupListsWithIrq(int iIrq);
void 	interrupt_wait_queue_lookup_lists_remove(int iIrq, THREAD_t* threadWaiting);

#endif /* OS_KERNEL_RESMGR_KY_RESMGR_H_ */
