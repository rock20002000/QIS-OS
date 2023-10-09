#include <types.h>
#include <libc/include/stdio.h>
#include <libc/include/iomsg.h>

#include <platform/include/syscall_vec.h>
#include <platform/include/time.h>
#include <platform/include/vga_rw.h>
#include <platform/include/msg.h>
#include <platform/include/kirk_task.h>
#include <ker_syscalls.h>

#include "aether_resmgr.h"
#include "kernel.h"
#include "debug.h"
#include "ipc.h"
#include "arch_task.h"
#include "ker_time.h"
#include "ltimer.h"
#include <semaphore.h>

extern char g_szSerialMsgTmp[];
extern int  g_iSerialMsgCur;


/*

char console_write_buffer[256];

#define MONBUF			((uintptr_t)0xb0000)
#define COLBUF			((uintptr_t)0xb8000)

#define BIOS_COL		(*(uint8_t *)0x450)
#define BIOS_ROW		(*(uint8_t *)0x451)
#define BIOS_COLUMNS	(*(uint16_t *)0x44a)

void
init_vga(unsigned channel, const char *init, const char *defaults) {
	uint16_t *vram = (uint16_t *) paddr2ptr(0xb8000);
	uint8_t cTmp = ' ';
	unsigned 		row, col, ncols, nrows;

    col = BIOS_COL;
    row = BIOS_ROW;

    ncols = BIOS_COLUMNS;
    if(ncols != 40  &&  ncols != 80) ncols = 80;
    nrows = 25;

	for(int y=0; y<nrows; y++) {
			for(int x=0; x<ncols; x++) {
				vram[y * ncols + x] = ((vram[y * ncols + x]&0xFF00) | cTmp);
			}
	}
}

int fInit = 0;

static void
scroll(unsigned char *dp, unsigned ncols) {
    memmove(dp, dp + 2*ncols, 2*24*ncols);
    dp += 2*24*ncols;
    while(ncols != 0) {
        *dp++ = 0x20;
        *dp++ = 0x07;
        --ncols;
    }
}

void
put_vga(int c) {
    unsigned char 	*dp;
    unsigned 		row, col, ncols, nrows;
    unsigned		off;

    if(fInit==0){
    	fInit = 1;
    	init_vga(0, NULL, NULL);
    }

    col = BIOS_COL;
    row = BIOS_ROW;

    ncols = BIOS_COLUMNS;
    if(ncols != 40  &&  ncols != 80) ncols = 80;
    nrows = 25;

    switch(c) {
    case '\n':
        ++row;
        col = 0;
        break;
    case '\r':
        col = 0;
        break;
    case 0x0c:
        row = col = 0;
        break;
    default:
        off = (row * ncols + col) * 2;
        dp = (unsigned char *) (MONBUF + off);
        dp[0] = c;
        dp[1] = 0x0f;
        dp = (unsigned char *) (COLBUF + off);
        dp[0] = c;
        dp[1] = 0x0f;
        ++col;
        break;
    }

    if (col >= ncols) {
        ++row;
        col = 0;
    }

    if(row >= nrows) {
        --row;
        scroll((unsigned char *)MONBUF, ncols);
        scroll((unsigned char *)COLBUF, ncols);
    }

    BIOS_COL = col;
    BIOS_ROW = row;
}

 */

static int kscr_write(uint8_t szTmp)
{
	put_vga(szTmp);	//	x64_screen_printchar(szTmp);
	if(szTmp=='\b'){
		printchar(NULL, '\b');	//printf("\b");
		printchar(NULL, ' ');
		printchar(NULL, '\b');
	}else{
		printf("%c", szTmp);
	}

	//	printk("ksys_write %d\n", num_bytes);
    //test to print via printk
    //if((NULL == target_fd)&&(1 == num_bytes))
    //{
    //    printk("%c", *src);
    //}
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////

static int ksys_write(const char* src, int size, void* target_fd)
{
	if (!target_fd)
	{
		for (int i = 0; i < size; i++)
		{
			printchar(NULL, src[i]);
		}
		return size;
	}
    return 0;
}

int enqueueEventPulse(struct sigevent* event){
	int iRslt = 0;
	//	if((cop = lookup_connect(tip->event.sigev_coid)) == NULL || cop->type != TYPE_CONNECTION || (chp = cop->channel) == NULL) {
	//		timer_free(prp, tip);
	//		return EBADF;
	//	}
	AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(event->coid);
	if((aetherConnTmp==NULL)	//||(aetherConnTmp->pThreadRelated!=CURRENT_THREAD)
			||(aetherConnTmp->nChannelID==-1)||(aetherConnTmp->pAetherDesc==NULL)) {
		//printf("No destination found for pulse event:0x%x.\n", event->coid);
		iRslt = ERR_NOT_FOUND;	//	-1;
	}else{
		PULSE_RECV_BLOCK_DESC* pRecvPulseBlockedThreadDesc = lookupReceivePulseBlockQueueLookupLists(aetherConnTmp->pAetherDesc);
		if(pRecvPulseBlockedThreadDesc==NULL){
			MSG_ITEM* pMsgItem 						= (MSG_ITEM*)kmallocz(sizeof(MSG_ITEM));
			pMsgItem->nChannelID 					= aetherConnTmp->nChannelID;
			pMsgItem->nConnectionID 				= event->coid;
			pMsgItem->payload.pulse.nPulseCode 		= event->code;
			pMsgItem->payload.pulse.nPulseValue 	= event->value;
			pMsgItem->payload.pulse.nPulsePriority 	= event->priority;

			aether_msg_queue_pushPulse(&(aetherConnTmp->pAetherDesc->msgQueue), pMsgItem);
			//printf("Put pulse event.\n");
		}else{
			copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->code), sizeof(uint16_t),  &(event->code));
			copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->value), sizeof(int),  	 &(event->value));

			setParamStackReturnValue(&(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs), EOK);	//	pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs.rax = EOK;	//	aetherConnTmp->nConnectionID;	//	The pulse don't need reply, no receive id in need

			//thread_runqueue_push(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);
			thread_runqueue_add_first(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);	//thread_runqueue_push(pRecvPulseBlockedThreadDesc);

			receive_pulse_block_queue_lookup_lists_remove(aetherConnTmp->pAetherDesc, pRecvPulseBlockedThreadDesc);
			//printf("Schedule pulse event.\n");
		}

	}

	return iRslt;
}


int createTimerCallBackfunction(void *arg)
{
	//paddr_t pageTableAddress = arch_pte_backup();

	//	if((cop = lookup_connect(tip->event.sigev_coid)) == NULL || cop->type != TYPE_CONNECTION || (chp = cop->channel) == NULL) {
	//		timer_free(prp, tip);
	//		return EBADF;
	//	}
	int ret = 0;
	struct sigevent* event = (struct sigevent*)arg;

	DBG("*********************createTimerCallBackfunction ticks:%u**************************\n",getTicks());

	ret = enqueueEventPulse(event);

	//removeTimer(p_newTimer);
	//list_nullify(&timersList);
	//arch_pte_restore(pageTableAddress);
	return ret;	//	0;
}

extern bool start_schedule;
long handle_syscall(iframe_t* iframe)	//, long a1, long a2, long a3, long a4, long a5)
{
	get_systime(&p_vdso_data->syscall_time);

    long n 	= getParamStackFirstValue(iframe);	//iframe->rdi;
    long a1 = getParamStackSecondValue(iframe);
    long a2 = getParamStackThirldValue(iframe);
    long a3 = getParamStackFourthValue(iframe);
    long a4 = getParamStackFifthValue(iframe);
    long a5 = getParamStackSixthValue(iframe);

//	long handle_syscall(int n, long a1, long a2, long a3, long a4, long a5)
//{
    long ret = EOK;

	//	int_handler_q* handler;

	//	uint16_t *vram = (uint16_t *) (0xffff800000000000+0xb8000);

	//	THREAD_t* pCurrentWorkThread = NULL;

	int		iSetSched = 0;	//	run schedule with syscall

	//	char 	szTmp[1024] = {0};

	switch(n)
    {
        case SYS_WRITE:
			{
#ifndef SYS_WRITE_BUFSIZE
 #define SYS_WRITE_BUFSIZE	(256)
#endif
				int size = SYS_WRITE_BUFSIZE;
				char buf[SYS_WRITE_BUFSIZE];

				if (a2 < size)
					size = a2;
				memcpy(buf, (void*)a1, size);

				paddr_t pageTableAddress = arch_pte_backup();

				ret = ksys_write(buf, size,(void*)a3);
				arch_pte_restore(pageTableAddress);
			}
            break;
        case SYSCALL_NR_WRITESCREEN:
			{
				uint8_t  a1Tmp = (uint8_t)a1;
				//int a2Tmp = a2;
				//long a3Tmp = a3;
				//long a4Tmp = a4;

				paddr_t pageTableAddress = arch_pte_backup();

				kscr_write(a1Tmp);	//,a2Tmp,a3Tmp,a4Tmp);
				arch_pte_restore(pageTableAddress);
				ret = EOK;
			}
        	break;
        case SYSCALL_NR_OPENAT:
        	{
				const char* pszPath = (char *) a1;
				const char* pszMode = (char *) a2;
				size_t pathLen = strlen(pszPath) + 1;
				size_t modeLen = strlen(pszMode) + 1;

				paddr_t pageTableAddress = arch_pte_backup();

				pszPath = (const char*)map_user_rdata(pszPath, pathLen);
				pszMode = (const char*)map_user_rdata(pszMode, modeLen);

				ret = msg_sys_open(pageTableAddress, pszPath, pszMode);	//FILE_DESCRIPTOR* fileTmp = ky_sys_open(prcTmp, pszPath, pszMode);

				unmap_user_data(pszMode, modeLen);
				unmap_user_data(pszPath, pathLen);

				arch_pte_restore(pageTableAddress);

				/*	FILE_DESCRIPTOR* fileDescTmp = (FILE_DESCRIPTOR*)kmallocz(sizeof(FILE_DESCRIPTOR));
				RESOURCE_DEVICE* resdevTmp = lookupResourceDeviceLookupListsWithDevName("sr", pszPath);

				fileDescTmp->dev_entry = resdevTmp;
				fileDescTmp->processApplication = lookupProcessWithPageTableAddress(pageTableAddress);
				fileDescTmp->dev_lock	= NULL;
				fileDescTmp->iStatus	= 0;
				fileDescTmp->fd = file_descriptor_lookup_lists_push(fileDescTmp);
				ret = fileDescTmp->fd;	*/

        	}
        	break;

        case SYSCALL_NR_CLOSE:
        	{
				int 		fd			= (int) 		a1;

				//int* 		piFD		= (int*) 		a1;
				//int 		fd 			= *piFD;

				paddr_t pageTableAddress = arch_pte_backup();

				ret = msg_sys_close(pageTableAddress, fd);	//FILE_DESCRIPTOR* fileTmp = ky_sys_open(prcTmp, pszPath, pszMode);
				arch_pte_restore(pageTableAddress);
			}
			break;

        case SYSCALL_MSG_SENDC:
			{
				int 		fd			= (int) 		a1;

				//int* 		piFD		= (int*) 		a1;
				//int 		fd 			= *piFD;

				MSG_DATA* 	pMsgSend 	= (MSG_DATA*) 	a2;
				int			iSendLen 	= (int) 		a3;
				MSG_DATA* 	pMsgReply 	= (MSG_DATA*) 	a4;
				int*		piReplyLen 	= (int*) 		a5;

				int			iMsgType 	= pMsgSend->iMsgType;

				paddr_t pageTableAddress = arch_pte_backup();


				if(iMsgType==MSG_READ){
					ret = msg_sys_read(pageTableAddress, fd, pMsgSend, iSendLen, pMsgReply, piReplyLen);		//	pMsgSend->pszMsg, pMsgSend->iMsgLen);
				}else if(iMsgType==MSG_WRITE){
					ret = msg_sys_write(pageTableAddress, fd, pMsgSend, iSendLen, pMsgReply, piReplyLen);	//	pMsgSend->pszMsg, pMsgSend->iMsgLen);
				}else{
					ret = ERR_INVALID_ARG;	//	-1;
				}

				arch_pte_restore(pageTableAddress);

				//	typedef struct _msg_wait_reply{
				//		THREAD* pThread;
				//		int		iDriverIndex;
				//		int		fdApplication;
				//		int 	iMsgType;
				//		int		iMsgLen;
				//		char* 	pszMsgSend;
				//		char*   pszMsgReply;
				//		int*    piMsgReply;
				//	}MSG_WAIT_REPLY;



			}
        	break;
        case SYSCALL_MSG_RECVC:
			{
				int 			iDriverIndex 			= (int) 		a1;

				//int* 			piDriverIndex 			= (int*) 		a1;
				//int 			iDriverIndex 			= *piDriverIndex;

				MSG_DATA* 		pMsgSend 				= (MSG_DATA*) 	a2;
				int				iSendLen 				= (int) 		a3;
				MSG_CONTEXT* 	pMsgContext 			= (MSG_CONTEXT*) a4;



				paddr_t pageTableAddress = arch_pte_backup();

				ret = msg_sys_recv(pageTableAddress, iDriverIndex, pMsgSend, iSendLen, pMsgContext);

				arch_pte_restore(pageTableAddress);

				//if(ret==-1){	//	Server should not be idle
				//	CURRENT_THREAD->state = STATE_SIGWAIT_INTR;	//	STATE_SIGWAITINFO;
				//}
			}
        	break;
        case SYSCALL_MSG_REPLYC:
			{
				int 			iDriverIndex 			= (int) 		a1;

				//int* 			piDriverIndex 			= (int*) 		a1;
				//int 			iDriverIndex 			= *piDriverIndex;

				MSG_DATA* 		pMsgReply 				= (MSG_DATA*) 	a2;
				int				iReplyLen 				= (int) 		a3;
				MSG_CONTEXT* 	pMsgContext 			= (MSG_CONTEXT*) a4;


				paddr_t pageTableAddress = arch_pte_backup();

				ret = msg_sys_reply(pageTableAddress, iDriverIndex, pMsgReply, iReplyLen, pMsgContext);

				arch_pte_restore(pageTableAddress);

			}
        	break;

        case SYSCALL_NR_WAITINTREVENT:
			{
				int iIrq = (int)a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

	        	if(CURRENT_THREAD->state != THREAD_STATE_SIGWAIT_INTREVENT){
		        	CURRENT_THREAD->state = THREAD_STATE_SIGWAIT_INTREVENT;	//	STATE_SIGWAITINFO;
		        	interrupt_wait_queue_lookup_lists_push(iIrq, CURRENT_THREAD);
	        	}

	        	arch_enable_irq(iIrq);

	        	iSetSched = start_sched_intr(iframe, 0, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
        	//	start_sched(); // The sched() should not exist in syscall
        	break;
        case SYSCALL_NR_NOTIFYINTREVENT:
			{
				int iIrq = (int)a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

	        	//asm_cli();
	        	THREAD_t* pThreadWaitTmp = lookupInterruptWaitQueueLookupListsWithIrq(iIrq);
	        	while(pThreadWaitTmp!=NULL){
	        		pThreadWaitTmp->state = THREAD_STATE_READY;	//	STATE_SIGWAITINFO;
	        		interrupt_wait_queue_lookup_lists_remove(iIrq, pThreadWaitTmp);
	        		pThreadWaitTmp = lookupInterruptWaitQueueLookupListsWithIrq(iIrq);
	        	}

	        	//asm_sti();
	        	arch_pte_restore(pageTableAddress);

	        	ret = EOK;
			}
        break;
        case SYSCALL_NR_YIELD:
			{
	        	paddr_t pageTableAddress = arch_pte_backup();

	        	iSetSched = start_sched_intr(iframe, 1, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
        	//	start_sched(); // The sched() should not exist in syscall
        	break;
        case SYSCALL_NR_SLEEP:
			{
				int iSleepInterval = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				SLEEP_THREAD_WAIT_WAKEUP* sleepTmp 	= (SLEEP_THREAD_WAIT_WAKEUP*)kmallocz(sizeof(SLEEP_THREAD_WAIT_WAKEUP));
				sleepTmp->iSleepInterval 	= iSleepInterval;
				sleepTmp->pThread			= CURRENT_THREAD;
				sleepTmp->pThread->state 	= THREAD_STATE_NANOSLEEP;	//	STATE_SIGWAITINFO;
				sleep_wait_wakeup_lookup_lists_push(sleepTmp);

	        	iSetSched = start_sched_intr(iframe, 0, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
        	//	start_sched(); // The sched() should not exist in syscall
        	break;
        case SYSCALL_NR_GETSTATE:
			{
				int iThreadID = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				//printf("Get status for thread:%d\n", iThreadID);
				if(iThreadID==0){
					ret = CURRENT_THREAD->state;
				}else{
					if(CURRENT_THREAD->process->threads.length>iThreadID){
						ret = CURRENT_THREAD->process->threads.storage[iThreadID]->state;
					}else{
						ret = THREAD_STATE_EXIT;
					}
				}

        		arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_NR_SCHEDULE:
        	{
        		int iThreadID = (int) a1;
        		//iThreadID = iThreadID -1;
        		if( iThreadID<0 ) iThreadID=0;

	        	paddr_t pageTableAddress = arch_pte_backup();

        		THREAD_t* pThreadToSchedule = thread_waitqueue_get(iThreadID);
        		if(pThreadToSchedule!=NULL){
            		pThreadToSchedule->state = THREAD_STATE_READY;
            		//if(pCurrentWorkThread!=NULL){
            		//	pCurrentWorkThread->state = THREAD_STATE_SIGWAIT_YIELD;
            		//}
        			//pCurrentWorkThread = pThreadToSchedule;
        			thread_runqueue_push(pThreadToSchedule);
        			thread_waitqueue_remove(pThreadToSchedule);
        		}else{
        			pThreadToSchedule = thread_fifo_waitqueue_get(iThreadID);
					if(pThreadToSchedule!=NULL){
						pThreadToSchedule->state = THREAD_STATE_READY;
						//if(pCurrentWorkThread!=NULL){
						//	pCurrentWorkThread->state = THREAD_STATE_SIGWAIT_YIELD;
						//}
						//pCurrentWorkThread = pThreadToSchedule;
						thread_fifo_runqueue_push(pThreadToSchedule);
						thread_fifo_waitqueue_remove(pThreadToSchedule);
					}
        		}

        		start_schedule = true;

        		arch_pte_restore(pageTableAddress);

        		ret = EOK;

        	}
        	break;
        case SYSCALL_SCHEDULE_NEXT:
			{
	        	paddr_t pageTableAddress = arch_pte_backup();

	        	iSetSched = start_sched_intr(iframe, 1, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
			break;
        case SYSCALL_PROCESS_CREATE:
			{
				const p_proc_create_attr_t usr_pca = (const p_proc_create_attr_t) a1;
				//	uint32_t map_nums = usr_pca->map.nums;
				size_t size = sizeof(*usr_pca) + (usr_pca->map.nums - 1) * sizeof(usr_pca->map.params[0]);
	        	paddr_t pageTableAddress = arch_pte_backup();


				THREAD_t* th = CURRENT_THREAD;
#ifdef KERNEL_VMAP_SIZE
			    p_proc_create_attr_t kpca = (p_proc_create_attr_t)
					kernel_map_user(th->process, usr_pca, size, KERNEL_MAP_RFLAGS);
#else
				p_proc_create_attr_t kpca =
					(p_proc_create_attr_t)copy_user_data(th->process, usr_pca, size);
#endif

				ret = load_proc(kpca);
#ifdef KERNEL_VMAP_SIZE
				kernel_unmap(kpca, size);
#else
				kfree(kpca);
#endif

				arch_pte_restore(pageTableAddress);

				ret = EOK;
			}
			break;
        case SYSCALL_PROCESS_DESTROY:
			{
				int i = 0;
	        	paddr_t pageTableAddress = arch_pte_backup();

				task_struct* proc = CURRENT_THREAD->process;

				DBG("Process exit for process:%s \n", proc->name);
				for(; i<proc->threads.length; i++){
					proc->threads.storage[i]->state = THREAD_STATE_EXIT;
				}
				proc->__state = PROCESS_STATE_ZOMBIE;
				// destroy_thread(CURRENT_THREAD, true);

	        	iSetSched = start_sched_intr(iframe, 1, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
			break;
        //case SYSCALL_INT_REGISTER:
        //	handler = (int_handler_q*) a1;
        //	long irqnum = a2;
        //	void* dev_id = (void *) a3;
        //	void* irqdata = (void *) a4;
        //	long  flag = a5;
        //	;//register_interrupt_handler2(irqnum, handler, dev_id,  irqdata, flag);
        //    break;

        case SYSCALL_THREAD_CREATE:
        	{
        		start_rtn_withParam thread_handler 	= (start_rtn_withParam) a1;
				void* thread_id 			= (void *) a2;
				void* threadAttrs 			= (void *) a3;
				void* threadParams 			= (void *) a4;
				void* funcUser 				= (void *) a5;
	        	paddr_t pageTableAddress    = arch_pte_backup();

				msg_pthread_create_withParam(CURRENT_THREAD, thread_id, threadAttrs, thread_handler, threadParams, funcUser, 1);

				//	ret = msg_pthread_create(pageTableAddress, thread_id, threadAttrs, thread_handler, threadParams);
				arch_pte_restore(pageTableAddress);

				ret = EOK;
        	}
        	break;
        case SYSCALL_THREAD_DESTROY:
			{
	        	paddr_t pageTableAddress = arch_pte_backup();

				DBG("Thread exit for process:%s \n", CURRENT_THREAD->process->name);

				CURRENT_THREAD->state = THREAD_STATE_EXIT;
				// destroy_thread(CURRENT_THREAD, true);

	        	iSetSched = start_sched_intr(iframe, 1, NULL);
	        	if (!iSetSched)
				{
					ret = ERR_ABORTED;
					arch_pte_restore(pageTableAddress);
				}
			}
			break;

        case SYSCALL_THREAD_CTL:
			{
				int cmd = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(cmd==THREAD_CTL_IO){
					CURRENT_THREAD->thread_settings.fIOAccess = 1;
				}
				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_THREAD_SETBLOCKING:
			{
				int iProcessID = (int) a1;
				int iPriority = (int) a2;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
					CURRENT_THREAD->priority = iPriority;
					CURRENT_THREAD->schedulePolicy 	= SCHEDULE_POLICY_FIFO;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0)){	//	if(CURRENT_THREAD->process->threads.length>iThreadID){
						ret = procTmp->threads.storage[0]->priority;
						procTmp->threads.storage[0]->priority = iPriority;

						if(procTmp->threads.storage[0]->schedulePolicy != SCHEDULE_POLICY_FIFO){
							if(NULL!=thread_waitqueue_get(procTmp->threads.storage[0]->tid)){
								thread_fifo_runqueue_push(procTmp->threads.storage[0]);
								thread_waitqueue_remove(procTmp->threads.storage[0]);
							}
						}

						procTmp->threads.storage[0]->schedulePolicy = SCHEDULE_POLICY_FIFO;
					}else{
						ret = 0;
					}
				}

				arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_THREAD_GETBLOCKING:
			{
				int iProcessID = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0))	{	//	iThreadID){
						ret = procTmp->threads.storage[0]->priority;
					}else{
						ret = -1;
					}
				}

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_THREAD_SETCRITICAL:
			{
				int iProcessID = (int) a1;
				int iPriority = (int) a2;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
					CURRENT_THREAD->priority = iPriority;
					CURRENT_THREAD->schedulePolicy 	= SCHEDULE_POLICY_RR;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0)){	//	if(CURRENT_THREAD->process->threads.length>iThreadID){
						ret = procTmp->threads.storage[0]->priority;
						procTmp->threads.storage[0]->priority = iPriority;

						if(procTmp->threads.storage[0]->schedulePolicy == SCHEDULE_POLICY_FIFO){
							if(NULL!=thread_fifo_waitqueue_get(procTmp->threads.storage[0]->tid)){
								thread_runqueue_push(procTmp->threads.storage[0]);
								thread_fifo_waitqueue_remove(procTmp->threads.storage[0]);
							}
						}

						procTmp->threads.storage[0]->schedulePolicy = SCHEDULE_POLICY_RR;
					}else{
						ret = 0;
					}
				}

				arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_THREAD_GETCRITICAL:
			{
				int iProcessID = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0))	{	//	iThreadID){
						ret = procTmp->threads.storage[0]->priority;
					}else{
						ret = -1;
					}
				}

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_THREAD_SETPRIORITY:
			{
				int iProcessID = (int) a1;
				int iPriority = (int) a2;
	        	paddr_t pageTableAddress = arch_pte_backup();


				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
					CURRENT_THREAD->priority 		= iPriority;
					CURRENT_THREAD->schedulePolicy 	= SCHEDULE_POLICY_NORMAL;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0))	{	//	iThreadID){
						ret = procTmp->threads.storage[0]->priority;
						procTmp->threads.storage[0]->priority 		= iPriority;

						if(procTmp->threads.storage[0]->schedulePolicy == SCHEDULE_POLICY_FIFO){
							if(NULL!=thread_fifo_waitqueue_get(procTmp->threads.storage[0]->tid)){
								thread_runqueue_push(procTmp->threads.storage[0]);
								thread_fifo_waitqueue_remove(procTmp->threads.storage[0]);
							}
						}

						procTmp->threads.storage[0]->schedulePolicy = SCHEDULE_POLICY_NORMAL;
					}else{
						ret = -1;
					}

					/*if(CURRENT_THREAD->process->threads.length>0){	//	iThreadID){
						ret = CURRENT_THREAD->process->threads.storage[0]->priority;
						CURRENT_THREAD->process->threads.storage[0]->priority = iPriority;
					}else{
						ret = 0;
					}*/
				}

				arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_THREAD_GETPRIORITY:
			{
				int iProcessID = (int) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(iProcessID==0){
					ret = CURRENT_THREAD->priority;
				}else{
					PROCESS_t* procTmp = getProcessWithPid(iProcessID);
					if((procTmp!=NULL)&&(procTmp->threads.length>0))	{	//	iThreadID){
						ret = procTmp->threads.storage[0]->priority;
					}else{
						ret = -1;
					}
				}

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_INTERRUPT_ATTACH:
        	{
        		start_rtn_withParam thread_handler 	= (start_rtn_withParam) a1;
				void* thread_id 			= (void *) a2;
				void* threadAttrs 			= (void *) a3;
				intptr_t iIrq 				= (intptr_t) a4;
				void*  funcUser				= (void *) a5;
	        	paddr_t pageTableAddress    = arch_pte_backup();

				msg_pthread_create_withParam(CURRENT_THREAD, thread_id, threadAttrs, thread_handler, (void*)iIrq, funcUser, 1);
				//	ret = msg_pthread_create(pageTableAddress, thread_id, threadAttrs, thread_handler, threadParams);

				//	arch_enable_irq(iIrq);

				arch_pte_restore(pageTableAddress);

				ret = EOK;

        	}
        	break;
        case SYSCALL_INTERRUPT_ATTACH_EVENT:
			{
				int iIrq 				 = (int)a1;
				struct sigevent *event 	 = (struct sigevent *)a2;
	        	paddr_t pageTableAddress = arch_pte_backup();

				event = (struct sigevent *)copy_user_data(CURRENT_THREAD->process, event, sizeof(struct sigevent) );
				interrupt_event_lookup_lists_push(iIrq, event);

				arch_pte_restore(pageTableAddress);

				ret = EOK;
			}
        	break;
        case SYSCALL_INTERRUPT_DETACH_EVENT:
			{
				int iIrq 				 = (int)a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				struct sigevent * pEvent = lookupInterruptEventLookupListsWithIrq(iIrq);
				while(pEvent!=NULL){
					interrupt_event_lookup_lists_remove(iIrq, pEvent);
					kfree(pEvent);
					pEvent = lookupInterruptEventLookupListsWithIrq(iIrq);
				}

				arch_pte_restore(pageTableAddress);

				ret = EOK;
			}
        	break;
        case SYSCALL_INTERRUPT_MASK:
			{
				int iIrq 					= (int) a1;
				arch_disable_irq(iIrq);

				ret = EOK;
			}
			break;
        case SYSCALL_INTERRUPT_UNMASK:
        	{
				int iIrq 					= (int) a1;
				arch_enable_irq(iIrq);

				ret = EOK;
			};
			break;

        case SYSCALL_TIMER_CREATE:
			{
				//	clockid_t id				= (clockid_t)a1;
				void* data					= (void*)a2;
				size_t size					= (size_t)a3;
	        	paddr_t pageTableAddress    = arch_pte_backup();

				if ( getTimerEventQueueLookupListsLength()>=50 ){
					ret = ERR_NO_MEMORY;
				} else{
					Timer* p_newTimer= createTimer(createTimerCallBackfunction, 20, data, size);
					//DBG(" p_newTimer->list:0x%p \n", &p_newTimer->list);
					//DBG("CREATE SUCCESS timer->list:0x%p \n", &timer->list);
					p_newTimer->iTimerID =resmgr_get_unique_random_int(0);
					//	addTimer(p_newTimer);
					timer_event_queue_lookup_lists_push(p_newTimer);
					ret = p_newTimer->iTimerID;
				}

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_TIMER_SETTIME:
			{
				timer_t id = (int)a1;
				const struct _itimer * itime = (const struct _itimer *)a3;
	        	paddr_t pageTableAddress = arch_pte_backup();

				Timer* pTimerTmp = lookupTimerEventQueueLookupListsWithTimerID(id);
				if(pTimerTmp!=NULL){
#ifdef KERNEL_VMAP_SIZE
					struct _itimer* itimerTmp = (struct _itimer*)kernel_map_user(
						CURRENT_THREAD->process, itime, sizeof(*itime), KERNEL_MAP_RFLAGS);
#else
					struct _itimer* itimerTmp =
						copy_user_data(CURRENT_THREAD->process, itime, sizeof(struct _itimer));
#endif

					if (itimerTmp)
					{
						initTimer(pTimerTmp, pTimerTmp->callback,
							itimerTmp->it_value.tv_sec*1000000+itimerTmp->it_value.tv_nsec/1000,
							pTimerTmp->data, pTimerTmp->size, 1);
						pTimerTmp->fValid = 1;
						addTimer(pTimerTmp);
#ifdef KERNEL_VMAP_SIZE
						kernel_unmap(itimerTmp, sizeof(*itimerTmp));
#else
						kfree(itimerTmp);
#endif
						ret = EOK;
					}
					else{
						ret = ERR_NO_MEMORY;
					}
				}else{
					ret = ERR_NOT_FOUND;
				}
				arch_pte_restore(pageTableAddress);


			}
			break;
        case SYSCALL_TIMER_DESTROY:
        	{
        		timer_t id = (int)a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				Timer* pTimerTmp = lookupTimerEventQueueLookupListsWithTimerID(id);
				if(pTimerTmp!=NULL){
					pTimerTmp->fValid = 0;
				}

				arch_pte_restore(pageTableAddress);
        	}
        	break;
        case SYSCALL_NRX_MODULE_LOAD:
			{
	        	int * 	pStatus		= (int*) 	a1;
	        	char* 	pszPath		= (char*)	a2;
	        	int  	iDevToken	= (int) 	a3;
	        	DEVICE_FUNCTIONS* pDevFunctions = (DEVICE_FUNCTIONS*) a4;

	        	void*   pFuncOpen	= pDevFunctions->devopen;
	        	void*   pFuncRead	= pDevFunctions->devread;
	        	void*   pFuncWrite	=  pDevFunctions->devwrite;
	        	void*   pFuncClose	=  pDevFunctions->devclose;
	        	size_t	iPathLen = strlen(pszPath) + 1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				//	pszPath = "/dev/ser1";
				pszPath = (char*)map_user_data(pszPath, iPathLen);
				pStatus = (int *)map_user_data(pStatus, sizeof(*pStatus));

				pFuncOpen	= map_user_text(pFuncOpen, 0);
			    pFuncRead	= map_user_text(pFuncRead, 0);
			    pFuncWrite	= map_user_text(pFuncWrite, 0);
			    pFuncClose	= map_user_text(pFuncClose, 0);

				ret = registerDevice(pageTableAddress, pStatus, pszPath, iDevToken, pFuncOpen, pFuncRead, pFuncWrite, pFuncClose);

				unmap_user_data(pszPath, iPathLen);
				arch_pte_restore(pageTableAddress);
				//devopenTmp = pFuncOpen;
				//devopenTmp("/dev/ser1", "rw");	//	pDevFunctions->devopen("/dev/ser1", "rw");
			}
			break;
        case SYSCALL_CLOCK_TIME:
			{
				struct timespec 	tp;
				struct tm	 		tm;
				clockid_t  			clock_id	= (clockid_t) 			a1;
				struct timespec* 	p_tp		= (struct timespec*)	a2;
				struct tm* 			p_tm		= (struct tm*)			a3;
	        	paddr_t pageTableAddress = arch_pte_backup();

				ret = ker_clock_gettime(clock_id, &tp, &tm);
				arch_pte_restore(pageTableAddress);
				if (EOK == ret)
				{
					if (p_tp)
						memcpy(p_tp, &tp, sizeof(tp));
					if (p_tm)
						memcpy(p_tm, &tm, sizeof(tm));
				}
			}
			break;
        case SYSCALL_CREATE_CHANNEL:
			{
				unsigned int  	flags		= (unsigned int) a1;
				char*  			pszPathTmp	= (char*) a2;
				uint32_t		pidTmp	= -1;
				size_t path_len		= 0;
				bool absolute_path	= false;

				if (pszPathTmp)
				{
					path_len = strlen(pszPathTmp);
					absolute_path = PATH_DIR_CHAR == pszPathTmp[0];
				}

	        	paddr_t pageTableAddress = arch_pte_backup();

				if(CURRENT_THREAD->process!=NULL) pidTmp = CURRENT_THREAD->process->pid;

				AETHER_DESC* aetherTmp 	= (AETHER_DESC*)kmallocz(sizeof(AETHER_DESC));

				aetherTmp->nPID					= CURRENT_THREAD->process->pid;
				aetherTmp->nChannelID			= resmgr_get_unique_random_int(1);	//	get_random_int();	//	aether_queue_lookup_lists_getLength(pidTmp);	//
				aetherTmp->nConnectionCount		= 0;		//	Connections
				aetherTmp->pThreadAether		= CURRENT_THREAD;
				aetherTmp->fValid				= 1;
				aetherTmp->nflags				= flags;

				DYNARRAY_NULLIFY(aetherTmp->recvMsgBlockThreadsQueue);
				DYNARRAY_NULLIFY(aetherTmp->recvPulseBlockThreadsQueue);

				//aetherTmp->pFileWithBus			= NULL;			//	File Descriptor
				//aetherTmp->pThreadsConnections	= NULL;		//  Threads for the connections
				//aetherTmp->pPkgListsConnections = NULL;	//  Threads for the connections

				ret = aetherTmp->nChannelID;
	
				if (path_len)
				{
					DEVICE_NODE* pDeviceNodeTmp 	= (DEVICE_NODE*)kmallocz(sizeof(DEVICE_NODE));
					pDeviceNodeTmp->iPid			= CURRENT_THREAD->process->pid;
					pDeviceNodeTmp->nChannelID		= aetherTmp->nChannelID;
					path_from_user(NULL, pszPathTmp,
						path_len+1, absolute_path, pDeviceNodeTmp->szDevicePath);
					device_node_lookup_lists_push(pDeviceNodeTmp);
					// KDBG("SYSCALL_CREATE_CHANNEL: %s, %d-%d\n",
					// 	pDeviceNodeTmp->szDevicePath, pDeviceNodeTmp->iPid, pDeviceNodeTmp->nChannelID);
					path_object_t obj = {.type=PATH_OBJECT_CHANNEL, .pid=pDeviceNodeTmp->iPid, .chid=pDeviceNodeTmp->nChannelID};
					error_t err = pathmgr_link(pDeviceNodeTmp->szDevicePath, &obj, false);
					if (EOK != err){
						KWARN("link: %s, failed, %d!", pDeviceNodeTmp->szDevicePath, err);
						kfree(aetherTmp);
						ret = EXIT_FAILURE;
					}else{
						aether_queue_lookup_lists_push(pidTmp, aetherTmp);
					}
						
				}else{
					aether_queue_lookup_lists_push(pidTmp, aetherTmp);
				}
				
				arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_CONNECT_ATTACH:
			{
				pid_t 		pid 		 = (pid_t)a2;
				int    		chid 		 = (int)a3;
	        	paddr_t pageTableAddress = arch_pte_backup();

				if(pid==0){
					if(CURRENT_THREAD->process!=NULL) pid = CURRENT_THREAD->process->pid;
				}
				if(pid==-1){
					pid = 0;
				}
				AETHER_CONNECTION_DESC* aetherConnTmp 	= (AETHER_CONNECTION_DESC*)kmallocz(sizeof(AETHER_CONNECTION_DESC));
				aetherConnTmp->pidConn					= pid;
				aetherConnTmp->nChannelID 				= chid;
				aetherConnTmp->nConnectionID 			= resmgr_get_unique_random_int(1);
				aetherConnTmp->pAetherDesc				= aetherQueueLookupListsWithIndex(pid, chid);
				aetherConnTmp->pThreadRelated			= CURRENT_THREAD;
				aetherConnTmp->fValid = 1;
				aether_queue_lookup_lists_connection_push(pid, chid, aetherConnTmp, CURRENT_THREAD);
				aether_connection_queue_lookup_lists_push(aetherConnTmp);

				ret = aetherConnTmp->nConnectionID;

				arch_pte_restore(pageTableAddress);

			}
        	break;
        case SYSCALL_DESTROY_CHANNEL:
			{
				uint32_t chidTmp = (uint32_t) a1;
	        	paddr_t pageTableAddress = arch_pte_backup();

				pid_t		pidTmp	= CURRENT_THREAD->process->pid;
				AETHER_DESC* aetherTmp = aetherQueueLookupListsWithIndex(pidTmp, chidTmp);
				if(aetherTmp!=NULL)	{
					if(aetherTmp->pThreadAether!=CURRENT_THREAD){
						ret = ERR_NOT_PERMITTED;	//	-1;
					}else{
						aether_queue_lookup_lists_remove(pidTmp, aetherTmp);	//	aetherTmp->fValid = 0;
						ret = EOK;
					}
				}
				//	All pending connections should be closed

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_CONNECT_DETACH:
			{
				uint32_t coidTmp = (uint32_t)a1;
	        	paddr_t pageTableAddress = arch_pte_backup();


				AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(coidTmp);

				if(aetherConnTmp!=NULL)	{
					if(aetherConnTmp->pThreadRelated!=CURRENT_THREAD){
						ret = ERR_NOT_PERMITTED;	//	-1;
					}else{
						aether_queue_lookup_lists_connection_remove(aetherConnTmp->pidConn, aetherConnTmp->nChannelID, aetherConnTmp);
						aether_connection_queue_lookup_lists_remove(aetherConnTmp);
						ret = EOK;
					}
				}else{
					ret = ERR_NOT_FOUND;
				}
				//	All pending messages and pulses should be cleared

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_MSG_SENDPULSE:
			{
				long iCoidTmp 		= (long)a1;
				long iPriorityTmp	= (long)a2;
				long iCodeTmp		= (long)a3;
				long iValueTmp		= (long)a4;
	        	paddr_t pageTableAddress = arch_pte_backup();

				ret = EOK;
				AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(iCoidTmp);
				if((aetherConnTmp==NULL)||(aetherConnTmp->pThreadRelated!=CURRENT_THREAD)
						||(aetherConnTmp->nChannelID==-1)||(aetherConnTmp->pAetherDesc==NULL)) {
					iSetSched = 0;
					ret = ERR_NOT_FOUND;	//	-1;
				}else{
					PULSE_RECV_BLOCK_DESC* pRecvPulseBlockedThreadDesc = lookupReceivePulseBlockQueueLookupLists(aetherConnTmp->pAetherDesc);
					if(pRecvPulseBlockedThreadDesc==NULL){
						MSG_ITEM* pMsgItem 						= (MSG_ITEM*)kmallocz(sizeof(MSG_ITEM));
						pMsgItem->nChannelID 					= aetherConnTmp->nChannelID;
						pMsgItem->nConnectionID 				= iCoidTmp;
						pMsgItem->payload.pulse.nPulseCode 		= iCodeTmp;
						pMsgItem->payload.pulse.nPulseValue 	= iValueTmp;
						pMsgItem->payload.pulse.nPulsePriority 	= iPriorityTmp;

						aether_msg_queue_pushPulse(&(aetherConnTmp->pAetherDesc->msgQueue), pMsgItem);

						iSetSched = 0;
					}else{
						copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->code), sizeof(uint16_t),  &iCodeTmp);
						copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->value), sizeof(int),  	 &iValueTmp);

						setParamStackReturnValue(&(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs), aetherConnTmp->nConnectionID);	//pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs.rax = aetherConnTmp->nConnectionID;

						receive_pulse_block_queue_lookup_lists_remove(aetherConnTmp->pAetherDesc, pRecvPulseBlockedThreadDesc);

						iSetSched = start_sched_intr(iframe, 1, pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);
						if (!iSetSched)
							ret = ERR_ABORTED;
					}

				}

				if(iSetSched == 0) arch_pte_restore(pageTableAddress);
			}
        	break;
        case SYSCALL_MSG_RECVPULSE:
			{
				int				iChidTmp	= (long)a1;
				struct _pulse * pPulse		= ( struct _pulse *)a2;
				int16_t			code		= 0;
				int32_t			value		= 0;
	        	paddr_t pageTableAddress    = arch_pte_backup();

				AETHER_DESC* aetherDescTmp = aetherQueueLookupListsWithIndex(CURRENT_THREAD->process->pid, iChidTmp);

				if ((aetherDescTmp==NULL)
					|| (aetherDescTmp->pThreadAether!=CURRENT_THREAD)
						|| (aetherDescTmp->nChannelID==NULL_ID)) {
					ret = ERR_INVALID_ARG;
				}else{
					MSG_ITEM* pPulseTmp = aether_msg_queue_getPulse(&aetherDescTmp->msgQueue);

					if(pPulseTmp==NULL){
						//	Set current thread to receive block
						//screen_printf("MsgRecvV Block\n");

						PULSE_RECV_BLOCK_DESC* pPulseBlockDesc 	= (PULSE_RECV_BLOCK_DESC*)kmallocz(sizeof(PULSE_RECV_BLOCK_DESC));


						pPulseBlockDesc->pPulse		= pPulse;
						pPulseBlockDesc->pThreadRecvPulseBlocked 	= CURRENT_THREAD;

						receive_pulse_block_queue_lookup_lists_push(aetherDescTmp, pPulseBlockDesc);	//	CURRENT_THREAD);
						//	int iPos = ...
						//char szTmp[1024] = {0};
						//sprintf(szTmp, "MsgRecvV for %d \n", iPos);
						//screen_printf(szTmp);	//	"MsgRecvV\n");//DBG("MsgRecvV\n");

						iSetSched = start_sched_intr(iframe, 0, NULL);
						if (!iSetSched)
						{
							ret = ERR_ABORTED;
						}
					}else{	//	if (pPulseTmp!=NULL){
						code = pPulseTmp->payload.pulse.nPulseCode;
						value = pPulseTmp->payload.pulse.nPulseValue;
						aether_msg_queue_removePulse(&(aetherDescTmp->msgQueue), pPulseTmp);
						ret = EOK;
					}
					//else{
					//	ret = ERR_EMPTY;
					//}
				}

				if(iSetSched==0){
					arch_pte_restore(pageTableAddress);
					if (EOK == ret && pPulse)
					{
						pPulse->code = code;
						pPulse->value = value;
					}
				}
			}
        	break;
        case SYSCALL_MSG_DELIVER_EVENT:
        	{
				long iCoidTmp 		= (long)a1;
				long iPriorityTmp	= (long)a2;
				long iCodeTmp		= (long)a3;
				long iValueTmp		= (long)a4;
	        	paddr_t pageTableAddress = arch_pte_backup();

				AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(iCoidTmp);
				if((aetherConnTmp==NULL)	//||(aetherConnTmp->pThreadRelated!=CURRENT_THREAD)
						||(aetherConnTmp->nChannelID==-1)||(aetherConnTmp->pAetherDesc==NULL)) {
					ret = ERR_NOT_FOUND;	//	-1;
				}else{
					PULSE_RECV_BLOCK_DESC* pRecvPulseBlockedThreadDesc = lookupReceivePulseBlockQueueLookupLists(aetherConnTmp->pAetherDesc);
					if(pRecvPulseBlockedThreadDesc==NULL){
						MSG_ITEM* pMsgItem 						= (MSG_ITEM*)kmallocz(sizeof(MSG_ITEM));
						pMsgItem->nChannelID 					= aetherConnTmp->nChannelID;
						pMsgItem->nConnectionID 				= iCoidTmp;
						pMsgItem->payload.pulse.nPulseCode 		= iCodeTmp;
						pMsgItem->payload.pulse.nPulseValue 	= iValueTmp;
						pMsgItem->payload.pulse.nPulsePriority 	= iPriorityTmp;

						aether_msg_queue_pushPulse(&(aetherConnTmp->pAetherDesc->msgQueue), pMsgItem);

						ret = EOK;
					}else{
						copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->code), sizeof(uint16_t),  &(iCodeTmp));
						copy_kernel_data(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->process, &(pRecvPulseBlockedThreadDesc->pPulse->value), sizeof(int),  	 &(iValueTmp));

						setParamStackReturnValue(&(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs), EOK);	//	pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked->regs.rax = EOK;	//	aetherConnTmp->nConnectionID;	//	The pulse don't need reply, no receive id in need

						//thread_runqueue_push(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);
						//thread_runqueue_add_first(pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);	//thread_runqueue_push(pRecvPulseBlockedThreadDesc);

						receive_pulse_block_queue_lookup_lists_remove(aetherConnTmp->pAetherDesc, pRecvPulseBlockedThreadDesc);

						start_sched_intr(iframe, 1, pRecvPulseBlockedThreadDesc->pThreadRecvPulseBlocked);
						iSetSched = 1;
					}

				}
				if(iSetSched==0){
					arch_pte_restore(pageTableAddress);
				}

			}
			break;
        case SYSCALL_MSG_SENDV:
			{
				int 		iCoidTmp	= (int) 		a1;

				iov_t* 		piov_s 		= (iov_t*) 		a2;
				int			sparts 		= (int) 		a3;
				iov_t* 		piov_r 		= (iov_t*) 		a4;
				int			rparts 		= (int) 		a5;
				int			i 			= 0;
				iov_t* 		piov_r_org 	= (iov_t*) 		a4;
	        	paddr_t pageTableAddress = arch_pte_backup();


				if(piov_s!=NULL){
					piov_s = (iov_t *)copy_user_data(CURRENT_THREAD->process, piov_s, sizeof(iov_t)*sparts );
				}

				if(piov_r!=NULL){
					piov_r = (iov_t *)copy_user_data(CURRENT_THREAD->process, piov_r, sizeof(iov_t)*rparts );
				}

				AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(iCoidTmp);
				if((aetherConnTmp==NULL)||(aetherConnTmp->pThreadRelated!=CURRENT_THREAD)
						||(aetherConnTmp->nChannelID==-1)||(aetherConnTmp->pAetherDesc==NULL)) {
					screen_printf("MsgSendV check failed\n");
					ret = ERR_NOT_FOUND;	//	-1;
				}else{
					//printf("MsgSendV\n");

					MSG_SENDRECV_BLOCK_DESC* pRecvMsgBlockedThreadDesc = lookupReceiveMsgBlockQueueLookupLists(aetherConnTmp->pAetherDesc);
					if(pRecvMsgBlockedThreadDesc==NULL){

						MSG_ITEM* pMsgItem 						= (MSG_ITEM*)kmallocz(sizeof(MSG_ITEM));
						pMsgItem->nChannelID 					= aetherConnTmp->nChannelID;
						pMsgItem->nConnectionID 				= iCoidTmp;
						pMsgItem->pSendBlockThread				= CURRENT_THREAD;
						pMsgItem->payload.msg.siov 				= piov_s;
						pMsgItem->payload.msg.sparts 			= sparts;
						pMsgItem->payload.msg.riov 				= piov_r;
						pMsgItem->payload.msg.rparts 			= rparts;
						pMsgItem->payload.msg.riov_org			= piov_r_org;

						aether_msg_queue_pushMsg(&(aetherConnTmp->pAetherDesc->msgQueue), pMsgItem);

			        	iSetSched = start_sched_intr(iframe, 0, NULL);
						if (!iSetSched)
						{
							ret = ERR_ABORTED;
						}

						//printf("MsgSendV iSetSched��%d\n", iSetSched);
					}else{

						aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked 	= CURRENT_THREAD;
						aetherConnTmp->msgSendBlockedThreadDesc.msg.riov		= piov_r;
						aetherConnTmp->msgSendBlockedThreadDesc.msg.rparts		= rparts;
						aetherConnTmp->msgSendBlockedThreadDesc.msg.siov		= piov_s;
						aetherConnTmp->msgSendBlockedThreadDesc.msg.sparts		= sparts;

						aetherConnTmp->msgSendBlockedThreadDesc.msg.riov_org	= piov_r_org;

						int iRpartsCopy = MIN(sparts, pRecvMsgBlockedThreadDesc->msg.rparts);
						for(i=0; i<iRpartsCopy; i++ ){
							int iRpartsCopyLen = MIN(pRecvMsgBlockedThreadDesc->msg.riov[i].iov_len, piov_s[i].iov_len);

							//sprintf(szTmp, "user copy user: des len:%d src len:%d iRpartsCopyLen:%d \n", pRecvBlockedThreadDesc->msg.riov[i].iov_len, piov_s[i].iov_len, iRpartsCopyLen);
							//screen_printf(szTmp);
							//memcpy(pRecvThreadDesc->msg.riov[i].iov_base, piov_s[i].iov_base, iRpartsCopyLen);//piov_s[i].iov_len);
							ret = user_copy_user(pRecvMsgBlockedThreadDesc->pThreadSendRecvBlocked->process, pRecvMsgBlockedThreadDesc->msg.riov[i].iov_base, iRpartsCopyLen, CURRENT_THREAD->process, piov_s[i].iov_base);

							//sprintf(szTmp, "Return value:%d \n", ret);
							//screen_printf(szTmp);

							if(ret!=EOK) break;
							copy_kernel_data(pRecvMsgBlockedThreadDesc->pThreadSendRecvBlocked->process, &(pRecvMsgBlockedThreadDesc->msg.riov_org[i].iov_len), sizeof(int),  &iRpartsCopyLen);	//	pRecvBlockedThreadDesc->msg.riov[i].iov_len = iRpartsCopyLen;	//piov_s[i].iov_len;
						}

						int iLenForEmpty = 0;
						for(; i<pRecvMsgBlockedThreadDesc->msg.rparts; i++){
							copy_kernel_data(pRecvMsgBlockedThreadDesc->pThreadSendRecvBlocked->process, &(pRecvMsgBlockedThreadDesc->msg.riov_org[i].iov_len), sizeof(int),  &iLenForEmpty);
						}

						//printf("MsgSendV ret:%d\n", ret);

						if(ret==EOK){
							MSG_SENDRECV_BLOCK_DESC tmpMsgBlockDesc;

							setParamStackReturnValue(&(pRecvMsgBlockedThreadDesc->pThreadSendRecvBlocked->regs), aetherConnTmp->nConnectionID);	//	pRecvMsgBlockedThreadDesc->pThreadSendRecvBlocked->regs.rax = aetherConnTmp->nConnectionID;

							memcpy(&tmpMsgBlockDesc, pRecvMsgBlockedThreadDesc, sizeof(MSG_SENDRECV_BLOCK_DESC));
							kfree(pRecvMsgBlockedThreadDesc);
							receive_msg_block_queue_lookup_lists_remove(aetherConnTmp->pAetherDesc, pRecvMsgBlockedThreadDesc);
				        	iSetSched = start_sched_intr(iframe, 0, tmpMsgBlockDesc.pThreadSendRecvBlocked);	//pRecvBlockedThreadDesc->pThreadBlocked);
							if (!iSetSched)
							{
								ret = ERR_ABORTED;
							}
							//printf("MsgSendV iSetSched��%d\n", iSetSched);
						}else{
							iSetSched = 0;
						}
					}
				}
				if(iSetSched==0)arch_pte_restore(pageTableAddress);

			}
        	break;
        case SYSCALL_MSG_RECVV:
			{

//#define MAX_RESMGR_SRPARTS	7
				uint32_t		iChidTmp	= (uint32_t) 	a1;		//	(long)chid, (long)riov, (long)rparts, (long)info
				iov_t* 			piov_r 		= (iov_t*) 		a2;
				int				rparts 		= (int) 		a3;
				//MSG_PAYLOAD_ITEM* 	pMsgItem 		= (MSG_PAYLOAD_ITEM*) 	a4;
				int				i			= 0;
				//void*			tmpPos		= NULL;
				iov_t* 			piov_r_org	= (iov_t*) 		a2;
				int				rparts_org	= (int) 		a3;

				if(rparts>MAX_MSG_SRPARTS) ret   = ERR_NO_MEMORY;	//	-1;
				else{
	        		paddr_t pageTableAddress = arch_pte_backup();

					AETHER_DESC* aetherDescTmp = aetherQueueLookupListsWithIndex(CURRENT_THREAD->process->pid, iChidTmp);
					if((aetherDescTmp==NULL)||(aetherDescTmp->pThreadAether!=CURRENT_THREAD)
							||(aetherDescTmp->nChannelID==-1)) {
						ret = ERR_NOT_FOUND;	//	-1;
						screen_printf("MsgRecvV check failed\n");
					}else{
						MSG_ITEM* pMsgTmp = aether_msg_queue_getMsg(&aetherDescTmp->msgQueue);
						if(piov_r!=NULL){
							piov_r = (iov_t *)copy_user_data(CURRENT_THREAD->process, piov_r, sizeof(iov_t)*rparts );
						}

						if(pMsgTmp==NULL){
							//	Set current thread to receive block
							//screen_printf("MsgRecvV Block\n");

							MSG_SENDRECV_BLOCK_DESC* pMsgBlockDesc 	= (MSG_SENDRECV_BLOCK_DESC*)kmallocz(sizeof(MSG_SENDRECV_BLOCK_DESC));
							pMsgBlockDesc->msg.rparts		= rparts;
							pMsgBlockDesc->msg.riov			= piov_r;
							pMsgBlockDesc->msg.sparts		= 0;
							pMsgBlockDesc->msg.siov			= NULL;

							pMsgBlockDesc->msg.riov_org		= piov_r_org;
							pMsgBlockDesc->pThreadSendRecvBlocked 	= CURRENT_THREAD;

							receive_msg_block_queue_lookup_lists_push(aetherDescTmp, pMsgBlockDesc);	//	CURRENT_THREAD);

							//	int iPos = ...
							//char szTmp[1024] = {0};
							//sprintf(szTmp, "MsgRecvV for %d \n", iPos);
							//screen_printf(szTmp);	//	"MsgRecvV\n");//DBG("MsgRecvV\n");

				        	iSetSched = start_sched_intr(iframe, 0, NULL);
							if (!iSetSched)
							{
								ret = ERR_ABORTED;
							}
						} else{
							//screen_printf("MsgRecvV get send wait msg\n");
							if(rparts>pMsgTmp->payload.msg.sparts) rparts=pMsgTmp->payload.msg.sparts;
							for(i=0; i<rparts; i++){
								int iRpartsCopyLen = MIN(pMsgTmp->payload.msg.siov[i].iov_len, piov_r[i].iov_len);

								user_copy_user(CURRENT_THREAD->process, piov_r[i].iov_base, iRpartsCopyLen, pMsgTmp->pSendBlockThread->process, pMsgTmp->payload.msg.siov[i].iov_base);

								//	if(ret!=EOK)break;
								copy_kernel_data(CURRENT_THREAD->process, &(piov_r_org[i].iov_len), sizeof(int),  &iRpartsCopyLen);
							}

							int iLenForEmpty = 0;
							for(; i<rparts_org; i++){
								copy_kernel_data(CURRENT_THREAD->process, &(piov_r_org[i].iov_len), sizeof(int),  &iLenForEmpty);
							}
							//	if(ret==EOK)	//	The receive operation can't failed.
							ret = pMsgTmp->nConnectionID;
						}
						//	aether_msg_queue_removePulse(&(aetherDescTmp->msgQueue), pPulseTmp);
					}
					if( iSetSched == 0 ) arch_pte_restore(pageTableAddress);
				}
			}
        	break;
        case SYSCALL_MSG_REPLYV:
			{
				//	( int rcvid, int status, const iov_t* riov, int rparts )
				int 			iRcvid 		= (int) 	a1;
				int 			iStatus 	= (int) 	a2;
				iov_t*			piov_r		= (iov_t*)	a3;
				int 			rparts 		= (int) 	a4;
				int				i			= 0;
	        	paddr_t pageTableAddress    = arch_pte_backup();

				AETHER_CONNECTION_DESC* aetherConnTmp = aetherConnectionQueueLookupListsWithIndex(iRcvid);
				if((aetherConnTmp==NULL)
						||(aetherConnTmp->nChannelID==-1)||(aetherConnTmp->pAetherDesc==NULL)||(aetherConnTmp->pAetherDesc->nPID!=CURRENT_THREAD->process->pid)) {
					screen_printf("MsgReplyV Check failed\n");//DBG("MsgReplyV\n");

					ret = ERR_NOT_FOUND;	//	-1;
				}else{
					//screen_printf("MsgReplyV\n");//DBG("MsgReplyV\n");

					MSG_ITEM* pMsgTmp = aether_msg_queue_getMsg(&aetherConnTmp->pAetherDesc->msgQueue);	//		//	The block send thread would not queue another msg item

					if(piov_r!=NULL) piov_r = (iov_t *)copy_user_data(CURRENT_THREAD->process, piov_r, sizeof(iov_t)*rparts );

					if((pMsgTmp==NULL)||(pMsgTmp->nConnectionID!=iRcvid)){  // Not from msg queue, the sending thread just blocking
						//screen_printf("MsgReplyV direct\n");
						if(aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked !=	NULL)
						{
							if(rparts>aetherConnTmp->msgSendBlockedThreadDesc.msg.rparts) rparts=aetherConnTmp->msgSendBlockedThreadDesc.msg.rparts;
							for(i=0; i<rparts; i++){
								int iRpartsCopyLen = MIN(aetherConnTmp->msgSendBlockedThreadDesc.msg.riov[i].iov_len, piov_r[i].iov_len);

								user_copy_user(aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked->process, aetherConnTmp->msgSendBlockedThreadDesc.msg.riov[i].iov_base, iRpartsCopyLen, CURRENT_THREAD->process, piov_r[i].iov_base);

								copy_kernel_data(aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked->process, &(aetherConnTmp->msgSendBlockedThreadDesc.msg.riov_org[i].iov_len), sizeof(int),  &iRpartsCopyLen);
								//	aetherConnTmp->msgSendBlockedThreadDesc.msg.riov[i].iov_len = iRpartsCopyLen;
							}

							int iLenForEmpty = 0;
							for(; i<aetherConnTmp->msgSendBlockedThreadDesc.msg.rparts; i++){
								copy_kernel_data(aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked->process, &(aetherConnTmp->msgSendBlockedThreadDesc.msg.riov_org[i].iov_len), sizeof(int),  &iLenForEmpty);
							}
							aetherConnTmp->msgSendBlockedThreadDesc.pThreadSendRecvBlocked =	NULL;
							//start_sched_intr(iframe, 1, aetherConnTmp->msgSendBlockedThreadDesc.pThreadBlocked);
				        	iSetSched = 1;
							ret = EOK;
						}else{

							//screen_printf("MsgRecvV no send thread blocked\n");
							iSetSched = 0;
							ret = ERR_NOT_FOUND;
						}

					} else{

						if(rparts>pMsgTmp->payload.msg.rparts) rparts=pMsgTmp->payload.msg.rparts;
						for(i=0; i<rparts; i++){
							int iRpartsCopyLen = MIN(pMsgTmp->payload.msg.riov[i].iov_len, piov_r[i].iov_len);

							user_copy_user(pMsgTmp->pSendBlockThread->process, pMsgTmp->payload.msg.riov[i].iov_base, iRpartsCopyLen, CURRENT_THREAD->process, piov_r[i].iov_base);

							///	??? the address of iov_len can get or not ???
							copy_kernel_data(pMsgTmp->pSendBlockThread->process, &(pMsgTmp->payload.msg.riov_org[i].iov_len), sizeof(int),  &iRpartsCopyLen);
							//pMsgTmp->payload.msg.riov[i].iov_len = iRpartsCopyLen;
						}

						int iLenForEmpty = 0;
						for(; i<pMsgTmp->payload.msg.rparts; i++){
							copy_kernel_data(pMsgTmp->pSendBlockThread->process, &(pMsgTmp->payload.msg.riov_org[i].iov_len), sizeof(int),  &iLenForEmpty);
						}

						aether_msg_queue_removeMsg(&(aetherConnTmp->pAetherDesc->msgQueue), pMsgTmp);

						iSetSched = 1;
						ret = EOK;

						kfree(piov_r);
					}


					if(iSetSched==1){
						setParamStackReturnValue(&(aetherConnTmp->pThreadRelated->regs), iStatus);	//	aetherConnTmp->pThreadRelated->regs.rax = iStatus;	//rparts;
						iSetSched = start_sched_intr(iframe, 1, aetherConnTmp->pThreadRelated);
						if (!iSetSched)
						{
							ret = ERR_ABORTED;
						}
					}
				}
				if( iSetSched == 0 ) arch_pte_restore(pageTableAddress);

			}
        	break;

        case SYSCALL_PATHMGR_LINK:
		{
			const char *user_path		= (char*)a1;
			size_t path_len				= 0;
			bool absolute_path			= false;
			path_object_t obj;


			if (user_path)
			{
				path_len = strlen(user_path);
				absolute_path = PATH_DIR_CHAR == user_path[0];
				if (path_len)
				{
					obj.type = (int)a2;
					obj.handle = (handle_t)a3;
				}
			}

        	paddr_t pageTableAddress = arch_pte_backup();

			ret = ERR_INVALID_ARG;
			if (path_len)
			{
				char* path = path_from_user(NULL, user_path, path_len+1, absolute_path, NULL);

				if (path)
				{
					obj.pid = CURRENT_THREAD->process->pid;
					ret = pathmgr_link(path, &obj, a4 != 0);
				}
			}
			arch_pte_restore(pageTableAddress);
		}
		break;

        case SYSCALL_PATHMGR_UNLINK:
		{
			const char *user_path	= (char*)a1;
			size_t path_len			= 0;
			bool absolute_path		= false;

			if (user_path)
			{
				path_len = strlen(user_path);
				absolute_path = PATH_DIR_CHAR == user_path[0];
			}

        	paddr_t pageTableAddress = arch_pte_backup();

			ret = ERR_NOT_FOUND;
			if (path_len)
			{
				char* path = path_from_user(NULL, user_path, path_len+1, absolute_path, NULL);

				if (path)
				{
					ret = pathmgr_unlink(path);
				}
			}
			arch_pte_restore(pageTableAddress);
		}
		break;

        case SYSCALL_PATHMGR_REQ:
			{
				const char *user_path 	= (char*)a1;
				path_object_t* p_obj	= (path_object_t*)a2;
				size_t path_len			= 0;
				bool absolute_path		= false;
				path_object_t tmp_obj;

				if (user_path)
				{
					path_len = strlen(user_path);
					absolute_path = PATH_DIR_CHAR == user_path[0];
				}

        		paddr_t pageTableAddress = arch_pte_backup();

				ret = ERR_NOT_FOUND;
				if (path_len)
				{
					char* path = path_from_user(NULL, user_path, path_len+1, absolute_path, NULL);

					if (path)
					{
						const path_object_t* obj = pathmgr_object(path);

						if (obj)
						{
							memcpy(&tmp_obj, obj, sizeof(*obj));
							ret = EOK;
						}
						// if (ret == EOK)
						// 	KDBG("SYSCALL_PATHMGR_REQ: %s, %d-%d\n", path, obj.pid, obj.chid);
						// else
						// 	KDBG("SYSCALL_PATHMGR_REQ: %s not found\n", path);
						kfree(path);
					}
				}
				arch_pte_restore(pageTableAddress);
				if (ret == EOK && p_obj)
				{
					memcpy(p_obj, &tmp_obj, sizeof(tmp_obj));
				}
			}
			break;
        case SYSCALL_PATHOBJ_OPEN:
			{
				const char *user_path 	= (char*)a1;
				size_t path_len			= 0;
				bool absolute_path		= false;

				if (user_path)
				{
					path_len = strlen(user_path);
					absolute_path = PATH_DIR_CHAR == user_path[0];
				}

        		paddr_t pageTableAddress = arch_pte_backup();

				ret = ERR_NOT_FOUND;
				if (path_len)
				{
					char* path = path_from_user(NULL, user_path, path_len+1, absolute_path, NULL);

					if (path)
					{
						ret = pathobj_open(path);
						kfree(path);
					}
				}
				arch_pte_restore(pageTableAddress);
			}
			break;
		case SYSCALL_PATHOBJ_CLOSE:
			{
				int fd = (int)a1;
        		paddr_t pageTableAddress = arch_pte_backup();

				ret = pathobj_close(fd);
				arch_pte_restore(pageTableAddress);
			}
			break;
		case SYSCALL_PATHOBJ_CTRL:
			{
				int fd				= (int)a1;
				const void* p_user	= (const void*)a2;
				size_t size			= (size_t)a3;
        		paddr_t pageTableAddress = arch_pte_backup();

#ifdef KERNEL_VMAP_SIZE
				void* param = kernel_map_user(NULL, p_user, size, KERNEL_MAP_RFLAGS);
#else
				void* param = copy_user_data(NULL, p_user, size);
#endif

				if (param)
				{
					ret = pathobj_ioctl(fd, param, size);
#ifdef KERNEL_VMAP_SIZE
					kernel_unmap(param, size);
#else
					kfree(param);
#endif
				}
				else
					ret = ERR_NO_MEMORY;
				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_MMAP:
			{
				vaddr_t vaddr = a1 ? (vaddr_t)a1 : NULL_VADDR;
				size_t len = (size_t)a2;
				uint32_t flags = (uint32_t)a3;
				id_t id = (id_t)a4;
				offset_t off = (offset_t)a5;
        		paddr_t pageTableAddress = arch_pte_backup();

				THREAD_t* th = CURRENT_THREAD;

				ret = (long)MAP_FAILED;
				if(th->is_user_thread)
				{
					extern int check_thread_privilege(THREAD_t*, THREAD_SETTING_INDEX);
					if(check_thread_privilege(th, 0))
					{
						//DBG("mmap(%p, %x, %x, %x, %p)\n", vaddr, len, flags, id, off);
						if (EOK == vm_map(th->process, &vaddr, NULL_PADDR, &len, flags, id, off))
							ret = (long)vaddr;
						//DBG("mmap ret: %p\n", ret);

					}
				}
				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_MUNMAP:
			{
				vaddr_t vaddr = (vaddr_t)a1;
				size_t len = (size_t)a2;
        		paddr_t pageTableAddress = arch_pte_backup();

				THREAD_t* th = CURRENT_THREAD;
				
				//	DBG("munmap(%p, %x)\n", vaddr, len);
				ret = vm_unmap(th->process, vaddr, len);

				arch_pte_restore(pageTableAddress);
			}
			break;
        case SYSCALL_MOFFSET:
			{
				vaddr_t vaddr = (vaddr_t)a1;
				size_t len = (size_t)a2;
				shm_block_t shmb;
        		paddr_t pageTableAddress = arch_pte_backup();

				THREAD_t* th = CURRENT_THREAD;

				// DBG("moffset(%p, %x)\n", vaddr, len);
				ret = vm_offset(th->process, vaddr, len, &shmb);
				arch_pte_restore(pageTableAddress);

				if (EOK == ret)
				{
					if (a3)
						*(offset_t*)a3 = shmb.paddr;
					if (a4)
						*(size_t*)a4 = shmb.size;
				}
			}
			break;
        case SYSCALL_KER_NOP:
        	ret = EOK;
        	break;
			
			
		case SYSCALL_SEMAPHORE_CREATE:
			{			
				//	unsigned __type= (unsigned)a1;
				sem_t* semp=(sem_t*)a2;
				struct _sync_attr* attr = (struct _sync_attr* )a3;	
        		paddr_t pageTableAddress = arch_pte_backup();

				sem_t* sempTmp = copy_user_data(CURRENT_THREAD->process, semp, sizeof(sem_t) );	//	p_proc_create_attr_t kpca = (p_proc_create_attr_t)
				struct _sync_attr* attrTmp = copy_user_data(CURRENT_THREAD->process, attr, sizeof(struct _sync_attr) );	//	p_proc_create_attr_t kpca = (p_proc_create_attr_t)
			//	THREAD_t* th = CURRENT_THREAD;
				DBG("semp sempTmp->parent.parent.name:%s\n", sempTmp->sem->parent.parent.name);

				//sem_init(sempTmp, attrTmp->__protocol, attrTmp->__flags);
				sem_init(sempTmp,  attrTmp->__flags, attrTmp->__protocol);
				//sempHandler =resmgr_get_unique_random_int();
				//copy_kernel_data(NULL, &(semp->semHander), sizeof(int), &sempHandler);
				copy_kernel_data(NULL, &(semp->sempHandler), sizeof(phys_addr_t), &sempTmp);
			//	kfree(sempTmp);
				arch_pte_restore(pageTableAddress);
			}
			break;

		case SYSCALL_SEMAPHORE_OPEN:
			{
			
				char *name = (char*)a1;	
			    int oflag = (int)a2;
				short	mode = (short)a3;
				unsigned int value =  (unsigned int)a4;
				int size = (int)a5;

			//	DBG("sem name:%s\n", name);

				sem_t* semp;
        		paddr_t pageTableAddress = arch_pte_backup();

				char* sempstr = copy_user_data(CURRENT_THREAD->process, name, size );	//	p_proc_create_attr_t kpca = (p_proc_create_attr_t)
			//	DBG("sem Name:%s size:%d\n", sempstr,size);
			//	THREAD_t* th = CURRENT_THREAD;
			    semp=sem_open(sempstr, oflag, mode, value);
			//	sem_open(semp, attr->__protocol,attr->__flags);

				ret = (phys_addr_t)semp;

				arch_pte_restore(pageTableAddress);
			}
			break;	

		case SYSCALL_SEMAPHORE_TAKE:
			{			
				sem_t* semp = (sem_t*)a1;			
        		paddr_t pageTableAddress = arch_pte_backup();

			//	THREAD_t* th = CURRENT_THREAD;
			 	ret = sem_take(semp);
			//	sem_open(semp, attr->__protocol,attr->__flags);

				arch_pte_restore(pageTableAddress);
			}
			break;	

			
		case SYSCALL_SEMAPHORE_POST:
			{			
				sem_t* semp = (sem_t*)a1;			
        		paddr_t pageTableAddress = arch_pte_backup();

			//	THREAD_t* th = CURRENT_THREAD;
			 	ret = sem_post(semp);
			//	sem_open(semp, attr->__protocol,attr->__flags);

				arch_pte_restore(pageTableAddress);
			}
			break;		
		case SYSCALL_SEMAPHORE_DESTROY:
			{			
				sem_t* semp = (sem_t*)a1;			
        		paddr_t pageTableAddress = arch_pte_backup();

			 	ret = sem_destroy(semp);

				arch_pte_restore(pageTableAddress);
			}
			break;	

		case SYSCALL_SEMAPHORE_SYNCVALUE:
			{			
				sem_t* semp = (sem_t*)a1;			
        		paddr_t pageTableAddress = arch_pte_backup();

			 	semValue(semp,&ret);

				arch_pte_restore(pageTableAddress);
			}
			break;	
		/*
        case SYSCALL_SYNC_CREATE:
			{
				pthread_mutex_t* pMutexID = (pthread_mutex_t*)a1;
				size_t len = (size_t)a2;

        		paddr_t pageTableAddress = arch_pte_backup();

				pthread_mutex_desc* pMutexDesc = (pthread_mutex_desc*)malloc(sizeof(pthread_mutex_desc) );

				pMutexDesc->nMutexID;
				pMutexDesc->count = 0;			//	0: unlocked, positive values: locked, possible waiters count
				lock_rel(pMutexDesc->wait_lock);
				pMutexDesc->pThreadMutexOwner = NULL;

				sync_wait_queue_lookup_lists_push(pMutexDesc);

				arch_pte_restore(pageTableAddress);
			}
			break;
		*/
		case SYSCALL_MUTEX_INIT:
			{
				//phy_mutex_t phy_mutex_create(const char *name, uint8_t flag);

			}
			break;
		case SYSCALL_MUTEX_LOCK:
			{
				//static err_t phy_mutex_take(phy_mutex_t mutex, uint32_t time)


//				THREAD_t* th = CURRENT_THREAD;
//				lock_rel(pMutexDesc->wait_lock);
//				pMutexDesc->pThreadMutexOwner = th;
			}
			break;
		case SYSCALL_MUTEX_TRYLOCK:
			{
				//err_t phy_mutex_trytake(phy_mutex_t mutex);
			}
			break;
        case SYSCALL_MUTEX_UNLOCK:
			{
				//err_t phy_mutex_release(phy_mutex_t mutex);

//				lock_rel(pMutexDesc->wait_lock);
//				pMutexDesc->pThreadMutexOwner = NULL;
			}
			break;
        case SYSCALL_MUTEX_DESTROY:
			{
				//err_t phy_mutex_delete(phy_mutex_t mutex);

			}
			break;

		case SYSCALL_PLATFORM_ARM_SERIAL_RECV:
			{
				char* pszRecv = (char*)a1;
        		paddr_t pageTableAddress = arch_pte_backup();

				pszRecv = map_user_data(pszRecv, 1024);
				sprintf(pszRecv, "%s", g_szSerialMsgTmp);
				memset(g_szSerialMsgTmp, 0, 1024);
				g_iSerialMsgCur = 0;

				unmap_user_data(pszRecv, 1024);
				arch_pte_restore(pageTableAddress);
			}
			break;

		case SYSCALL_GET_SYSINFO:
			{
				sysinfo_type_e type = (sysinfo_type_e)a1;
				uintptr_t param = (uintptr_t)a2;
				void* p_sysinfo = (void*)a3;
				size_t size = (size_t)a4;
        		paddr_t pageTableAddress = arch_pte_backup();

				ret = get_sysinfo(type, param, p_sysinfo, size);
				arch_pte_restore(pageTableAddress);
			}
			break;
		case SYSCALL_DEBUGSHOW_MEM:
			{
        		paddr_t pageTableAddress = arch_pte_backup();

				mem_info();
				arch_pte_restore(pageTableAddress);
			}
			break;
        default:
            break;
    }

	if(iSetSched==0){
		setParamStackReturnValue(iframe, ret);	//	iframe->rax = ret;
	}
    return ret;
}
