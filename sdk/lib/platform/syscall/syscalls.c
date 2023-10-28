#include <types.h>
#include <libc/include/string.h>
#include <arch_syscall.h>
#include <platform/include/platform.h>
#include <utils/include/print_macros.h>
#include <platform/include/msg.h>

#include <semaphore.h>
#include <libc/include/iomsg.h>
#include <stat.h>
//#include <platform/include/resmgr.h>
#include <elf_vDSO.h>
#include <vdso.h>

/*Here we define a test syscall with syscall No.1 :syswrite*/
error_t sys_write(const char *buf, size_t len, void* fd)
{
    return syscall(SYS_WRITE, (uintptr_t) buf, len, (uintptr_t)fd, 0, 0);
}


void threadShellForIntr(int iIrq, start_rtn_withParam userFunction){	//IRQ_HANDLER_PARAM* pIrqHandlerParam){
	int iIrqVec 				=  	iIrq;	//	pIrqHandlerParam->iIrqVec;
	start_rtn threadFunction 	= 	(start_rtn)userFunction;	//	pIrqHandlerParam->threadFunction;
	while(1){
		InterruptWait(iIrqVec, NULL);
		threadFunction();
		//	yield();	//	The yield() should not be called, as the InterruptWait() in need to be called here.
	}
}

void threadShellWithExit(void* pThreadParamUser, start_rtn_withParam pFunctionUser){
	void* pThreadParam = pThreadParamUser;
	start_rtn_withParam pThreadFunction = pFunctionUser;
	pThreadFunction(pThreadParam);

	pthread_exit(0);
}

/*
//	static void*  __attribute__((noinline)) user_int_register(int_handler_q* handler, size_t irqnum, void*  dev_id, void* irqdata, long flag, unsigned long offset)
void* user_int_register(int_handler_q* handler, size_t irqnum, void*  dev_id, void* irqdata, long flag, unsigned long offset)
{
	return (void*)syscall(SYSCALL_INT_REGISTER, (long)handler, irqnum, (long)dev_id, (long)irqdata, flag);
}
*/

//static void*  __attribute__((noinline)) AttachInterrupt(IRQ_HANDLER_PARAM* pIrqHandlerParam){	//int iIrqVec, start_rtn handler){
void*  AttachInterruptC(int iIrq, start_rtn userFunction){	//	IRQ_HANDLER_PARAM* pIrqHandlerParam){	//int iIrqVec, start_rtn handler){
	//	IRQ_HANDLER_PARAM irqHandlerParam;
	//	irqHandlerParam.iIrqVec 		= iIrqVec;
	//	irqHandlerParam.threadFunction 	= handler;
	//	return (void*)syscall(SYSCALL_THREAD_CREATE, (long)threadShell, NULL, NULL, (long)(pIrqHandlerParam), 0 );

	return (void*)syscall(SYSCALL_INTERRUPT_ATTACH, (long)threadShellForIntr, 0, 0, (long)(iIrq), (long)(userFunction) );
	//	return (void*)syscall(SYSCALL_INTERRUPT_ATTACH, (long)threadShellForIntr, NULL, NULL, (long)(pIrqHandlerParam), (long)(pIrqHandlerParam->iIrqVec) );
}

int InterruptAttach( int intr,
       const struct sigevent * (* handler)(void *, int),
       const void * area,
       int size,
       unsigned flags ){

	return (int)AttachInterruptC(intr, (start_rtn)handler);
}

int InterruptDetach( int id ){
	return 1;
}

int InterruptAttachEvent(int __intr, const struct sigevent *__event, unsigned __flags){
	return syscall(SYSCALL_INTERRUPT_ATTACH_EVENT, (long)(__intr), (long)(__event), (long)__flags, 0, 0 );
}

int InterruptDetachEvent( int __intr ){		//	, const struct sigevent *__event
	return syscall(SYSCALL_INTERRUPT_DETACH_EVENT, (long)(__intr), 0, 0, 0, 0 );
}

//	int InterruptAttach(int __intr, const struct sigevent *(*__handler)(void *__area, int __id), const void *__area, int __size, unsigned __flags){
//		return (void*)syscall(SYSCALL_INTERRUPT_ATTACH, (long)threadEventShell, NULL, NULL, (long)(pIrqHandlerParam), (long)(pIrqHandlerParam->iIrqVec) );
//	}

int InterruptMask( int intr, int id ){
	return syscall(SYSCALL_INTERRUPT_MASK, intr, 0, 0, 0, 0);
}

int InterruptUnmask( int intr, int id ){
	return syscall(SYSCALL_INTERRUPT_UNMASK, intr, 0, 0, 0, 0);
}

//	static void*  __attribute__((noinline)) pthread_create(pthread_t*  thread_id, void* threadAttrs, start_rtn handler, void* threadParams)
int pthread_create(pthread_t*  thread_id, void* threadAttrs, start_rtn_withParam handler, void* threadParams)
//		size_t irqnum, void* irqdata, long flag, unsigned long offset)
//	static void*  __attribute__((noinline)) pthread_create(int_handler_q* handler, size_t irqnum,
//        void*  dev_id, void* irqdata, long flag, unsigned long offset)
{
	//long int ret;
	//long int op = SYSCALL_THREAD_CREATE;
	long ret = syscall(SYSCALL_THREAD_CREATE, (long)threadShellWithExit, (long)thread_id, (long)threadAttrs, (long)(threadParams), (long)handler );
	xprintf("ThreadID:%d after pthread create\n", *thread_id);
	xprintf("==========================");
	return ret;

	return (void*)syscall(SYSCALL_THREAD_CREATE, (long)threadShellWithExit, (long)thread_id, (long)threadAttrs, (long)(threadParams), (long)handler );
	//	return (void*)syscall(SYSCALL_THREAD_CREATE, (long)handler, (long)thread_id, (long)threadAttrs, (long)(threadParams), 0);
}

int pthread_detach( pthread_t thread ){
	return 1;
}

void pthread_exit( void* value_ptr ){
	syscall(SYSCALL_THREAD_DESTROY, (long)value_ptr, 0, 0, 0, 0);
	//	while(1);
	return;
}

int pthread_join( pthread_t thread,  void** value_ptr ){
	int iTmp = syscall(SYSCALL_NR_GETSTATE, thread, 0, 0, 0, 0);
	while( iTmp != THREAD_STATE_EXIT ){
		iTmp = syscall(SYSCALL_NR_GETSTATE, thread, 0, 0, 0, 0);;
	}
	return 1;
}

#ifdef VDSO_SYSTIME_SYM
#ifdef __LP64__
 extern void* __vdsosym(const char *name);
 #define vdsoFindSymbol __vdsosym
#else
 extern void* __vdsosym32(const char *name);
 #define vdsoFindSymbol __vdsosym32
#endif
int vdsoGetSystime(systime_t* st)
{
    static size_t (*get_systime)(systime_t*, const vdso_data_t*, const void*) = NULL;

    if (!g_procEnv.systimer_freq)
        return ERR_NOT_SUPPORT;

    if (!get_systime)
    {
        get_systime = vdsoFindSymbol(VDSO_SYSTIME_SYM);
        if (!get_systime)
        {
            xprintf("vdsoFindSymbol(%s) failed!\n", VDSO_SYSTIME_SYM);
            return 0;
        }
    }

    return get_systime(st, (const vdso_data_t*)g_procEnv.vdso_data, (const void*)g_procEnv.vdso_text);
}
static int vdsoGetTime(struct timespec* tp)
{
    systime_t st;

    int err = vdsoGetSystime(&st);

    if (IS_ERROR(err))
        return err;

	size_t tv_sec = st.ticks / g_procEnv.systimer_freq;
	size_t tv_nsec = (st.ticks % g_procEnv.systimer_freq) * (1000000000 / g_procEnv.systimer_freq);

	if (st.cycles)
	{
		size_t cycles_per_time = g_procEnv.cycles_per_sec / g_procEnv.systimer_freq;

		tv_nsec += st.cycles * (1000000000 / g_procEnv.systimer_freq) / cycles_per_time;
		if (tv_nsec >= 1000000000)
		{
			tv_nsec -= 1000000000;
			tv_sec++;
		}
	}

	if (tp)
	{
		tp->tv_sec = tv_sec;
		tp->tv_nsec = tv_nsec;
	}

    return EOK;
}
#endif
int clock_gettime(clockid_t clock_id, struct timespec * tp)
{
#ifdef VDSO_SYSTIME_SYM
	if (CLOCK_REALTIME == clock_id)
	{
		if (IS_OK(vdsoGetTime(tp)))
			return EOK;
	}
#endif
	return (int)syscall(SYSCALL_CLOCK_TIME, (long)clock_id, (long)tp, 0, 0, 0);
}

struct tm * localtime(){
	struct timespec  tp;
	static struct tm ts;
	int			iTmp 		= 0;
	int			iTmpDay 	= 0;
	int			iExtFebDay 	= 0;
	syscall(SYSCALL_CLOCK_TIME, (long)0, (long)&tp, (long)&ts, 0, 0);
	int iMonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	iTmp = ts.tm_sec + tp.tv_sec;
	ts.tm_sec = iTmp%60;
	iTmp = iTmp / 60;

	if(iTmp>0){
		iTmp = ts.tm_min + iTmp;
		ts.tm_min = iTmp%60;
		iTmp = iTmp / 60;
	}

	if(iTmp>0){
		iTmp = ts.tm_hour + iTmp;
		ts.tm_min = iTmp%24;
		iTmp = iTmp / 24;
	}

	if(iTmp>0){
		iTmpDay = ts.tm_yday + iTmp;

		iExtFebDay = 0;
		if((ts.tm_year%100)==0){
			if((ts.tm_year%400)==0){
				iExtFebDay = 1;
			}
		}else{
			if((ts.tm_year%4)==0){
				iExtFebDay = 1;
			}
		}

		while(iTmpDay>(365+iExtFebDay)){
			ts.tm_year++;
			iTmpDay -= (365+iExtFebDay);

			iExtFebDay = 0;
			if((ts.tm_year%100)==0){
				if((ts.tm_year%400)==0){
					iExtFebDay = 1;
				}
			}else{
				if((ts.tm_year%4)==0){
					iExtFebDay = 1;
				}
			}
		}
		ts.tm_yday = iTmpDay;	//%(365+iExtFebDay);
		//	ts.tm_year = ts.tm_year + (iTmpDay/365);

		iTmpDay = ts.tm_wday + iTmp;
		ts.tm_wday = iTmpDay%7;
	}

	iTmp = ts.tm_yday;
	for(int i=0; i<12; i++){
		int iMonthDay = iMonthDays[i];
		if(i==1) iMonthDay += iExtFebDay;
		if(iTmp<=iMonthDay){
			ts.tm_mon 	= i+1;
			ts.tm_mday	= iTmp;
			break;
		}else{
			iTmp -= iMonthDays[i];
		}
	}

	return &ts;
}

int kernel_nop(){
	return syscall(SYSCALL_KER_NOP, 0, 0, 0, 0, 0);
}
//	static long int  __attribute__((noinline)) InterruptWait(int iIrqNo)
long int InterruptWait(int iIrqNo, int* piTimeOut)
{
	// int iTmp = 0;
	syscall(SYSCALL_NR_WAITINTREVENT, iIrqNo, 0, 0, 0, 0);
	//iTmp = syscall(SYSCALL_NR_GETSTATE, 0, 0, 0, 0, 0);
	//while( iTmp == THREAD_STATE_SIGWAIT_INTREVENT ){
	//	iTmp = syscall(SYSCALL_NR_GETSTATE, 0, 0, 0, 0, 0);;
	//}

	return 1;
}


//	static long int  __attribute__((noinline)) waitInterruptEvent(int iIrqNo)
long int waitInterruptEvent(int iIrqNo)
{
	syscall(SYSCALL_NR_WAITINTREVENT, iIrqNo, 0, 0, 0, 0);
	return 1;
}

//	static long int  __attribute__((noinline)) notifyInterruptEvent(int iIrqNo)
long int notifyInterruptEvent(int iIrqNo)
{
	return syscall(SYSCALL_NR_NOTIFYINTREVENT, iIrqNo, 0, 0, 0, 0);
}

//	static long int  __attribute__((noinline)) yield()
long int yield()
{
	//int iTmp = 0;
	//syscall(SYSCALL_NR_YIELD, 0, 0, 0, 0, 0);
	//iTmp = syscall(SYSCALL_NR_GETSTATE, 0, 0, 0, 0, 0);
	//while( iTmp == THREAD_STATE_SIGWAIT_YIELD ){
	//	iTmp = syscall(SYSCALL_NR_GETSTATE, 0, 0, 0, 0, 0);;
	//}
	//return 1;
	return syscall(SYSCALL_NR_YIELD, 0, 0, 0, 0, 0);
}

void msleep(int iMs)
{
	syscall(SYSCALL_NR_SLEEP, iMs, 0, 0, 0, 0);
}

void sleep(int iSec)
{
	msleep(iSec * 1000);
}

//	static long int  __attribute__((noinline)) scheduleToRun(int iThreadID)
long int scheduleToRun(int iThreadID)
{
	return syscall(SYSCALL_NR_SCHEDULE, iThreadID, 0, 0, 0, 0);
}

//	static long int  __attribute__((noinline)) exec(int iProcessIndex)
long int init_process_main_thread(const p_proc_create_attr_t pca)
{
    ASSERT(pca);
	return syscall(SYSCALL_PROCESS_CREATE, (long)pca, 0, 0, 0, 0);
}



int  shell_exec_id(int iProcessIndex)
{
	path_object_t obj;
    int iCoid 			= -1;
    //	int i 				= 0;
    int iRslt			= 0;

    char szTmp[1024] 	= {0};
    char buf[128] 		= {0};

    screen_printf("shell_exec_id\n");
    //if (argc < 3) {
    //        fprintf(stderr, "Usage: simple_client <pid> <chid>\n");
    //        return -1;
    //}
    //spid = atoi(argv[1]);
    //chid = atoi(argv[2]);


	int iRet = PathmgrRequire("procmgr", &obj);

	sprintf(szTmp, "Client: get pid = %d, chid = %x iRet:%d\n", obj.pid, obj.chid, iRet);
	screen_printf(szTmp);
	if(iRet!=EOK) return iRet;	//if(iRet==0) return -1;


    if ((iCoid = ConnectAttach(0, -1, obj.chid, 0, 0)) == -1) {
            perror("ConnectAttach");
            return -1;
    }
    memcpy(buf, &iProcessIndex, sizeof(int));	//sprintf(buf, "Hello");
    //printf_s("client: sent '%d' with coid:%x\n", *(int*)(&buf), iCoid);		//printf_s("client: sent '%s'\n", buf);

    iRslt = MsgSend(iCoid, buf, sizeof(int), buf, sizeof(buf));
    //	if (MsgSend(coid, buf, strlen(buf) + 1, buf, sizeof(buf)) != 0) {
    //        perror("MsgSend");
    //        ConnectDetach(coid);
    //        return -1;
    //	}
    //printf_s("client: returned '%d' and result:%d\n", *(int*)(&buf), iRslt);	//	printf_s("client: returned '%s'\n", buf);

    ConnectDetach(iCoid);

    return iRslt;
}

int  exec(char* pszCmdLine){
	shell_exec(pszCmdLine);
}

int  shell_exec(char* pszCmdLine)
{
	path_object_t obj;
    int iCoid 			= -1;
    //	int i 			= 0;
    int iRslt			= 0;

    //	char szTmp[1024] 	= {0};
    char szBuf[1024] 	= {0};
    int		iLength		= 0;
    //	screen_printf("shell_exec\n");

	int iRet = PathmgrRequire("procmgr", &obj);

	//	sprintf(szTmp, "Client: get pid = %d, chid = %x iRet:%d\n", obj.pid, obj.chid, iRet);
	//	screen_printf(szTmp);
	if(iRet!=OK) return iRet;	//if(iRet==0) return -1;

	iLength = strlen(pszCmdLine);

    if ((iCoid = ConnectAttach(0, -1, obj.chid, 0, 0)) == -1) {
            perror("ConnectAttach");
            return -1;
    }
    sprintf(szBuf, "%s", pszCmdLine);	//memcpy(szBuf, pszCmdLine, iLength);	//sprintf(buf, "Hello");
    //printf_s("client: sent '%d' with coid:%x\n", *(int*)(&buf), iCoid);		//printf_s("client: sent '%s'\n", buf);

    iRslt = MsgSend(iCoid, szBuf, iLength, szBuf, sizeof(szBuf));
    //sprintf(szTmp, "iRslt:%d\n", iRslt);
    //printf_s(szTmp);
    if(iRslt==EOK){
    	memcpy(&iRslt, szBuf, sizeof(int));
    	//sprintf(szTmp, "iRslt_loadProc:%d\n", iRslt);
    	//printf_s(szTmp);
    }
    ConnectDetach(iCoid);

    return iRslt;
}

void exit( int status )
{
	syscall(SYSCALL_PROCESS_DESTROY, (long)status, 0, 0, 0, 0);
	//	pthread_exit( &status );
}

int devRegister(int* pStatus, const char *pszFullPath, int devToken, DEVICE_FUNCTIONS* pDevFunctions){
	return syscall(SYSCALL_NRX_MODULE_LOAD, (long)pStatus, (long)pszFullPath, devToken, (long)pDevFunctions, 0);
}

volatile int iMsgRcvLen = 0;
MSG_DATA 	msgSend;
MSG_DATA 	msgRecv;
uint8_t		szRecvBuf[65536] = {0};

// int* __rbytes
int devMsgSendC(int __coid, const void *__smsg, void *__rmsg){	//	int devMsgSend(int __coid, const void *__smsg, int __sbytes, void *__rmsg, volatile int* __rbytes){
	//MSG_DATA* pSendMsg 	= (MSG_DATA*)__smsg;
	MSG_DATA* pReplyMsg = (MSG_DATA*)__rmsg;
	int __sbytes		= sizeof(MSG_DATA);

	iMsgRcvLen = 0;
	syscall(SYSCALL_MSG_SENDC, __coid, (long)__smsg, __sbytes, (long)__rmsg, (long)&iMsgRcvLen);	//	syscall(SYSCALL_MSG_SEND, __coid, __smsg, __sbytes, __rmsg, __rbytes);
	while(iMsgRcvLen==0){	//	((__rbytes!=NULL)&&(*__rbytes == 0)){	//CURRENT_THREAD->state!=STATE_RUNNING){
		;
	}
	return pReplyMsg->iMsgLen;	//	MSG_DONE;	//	((MSG_DATA*)__rmsg)->iMsgType;
	//	return syscall(SYSCALL_MSG_SEND, __coid, __smsg, __sbytes, __rmsg, __rbytes);
}

int devMsgRecvC(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info, int reserved){
	return syscall(SYSCALL_MSG_RECVC, (long)__chid, (long)__msg, (long)__bytes, (long)__info, (long)reserved);
}

int devMsgReplyC(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info){
	return syscall(SYSCALL_MSG_REPLYC, __chid, (long)__msg, __bytes, (long)__info, 0);
}

int devThreadWait(){
	return syscall(SYSCALL_THREAD_WAIT, 0, 0, 0, 0, 0);
}

int TimerCreate( clockid_t id, const struct sigevent *event )
{
	xprintf("TimerCreate: %p, c=%d, v=%d\n", event, event->code, event->value);
	return syscall(SYSCALL_TIMER_CREATE, (long)id, (long)event, (long)sizeof(*event), 0, 0);
}

int timer_create( clockid_t clock_id, struct sigevent * evp, timer_t * timerid )
{
	return(((*timerid = TimerCreate(clock_id, evp)) == -1) ? -1 : 0);
}

int TimerDestroy( timer_t id )
{
	return syscall(SYSCALL_TIMER_DESTROY, (long)id, 0, 0, 0, 0);
}

//int TimerInfo( pid_t pid, timer_t id, int flags,  struct _timer_info* info )
//{
//	return 1;
//}
int TimerSettime( timer_t id,  int flags,  const struct _itimer * itime, struct _itimer * oitime )
{
	return syscall(SYSCALL_TIMER_SETTIME, (long)id, (long)flags, (long)itime, (long)oitime, 0);
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue) {
	return TimerSettime(timerid, flags, (const struct _itimer*)value, (struct _itimer*)ovalue);

	/*struct _itimer		n, o;
	int					ret;

	if(value) {
		//POSIX 14.2.4.4 error checking
		if(!TIMESPEC_VALID(&value->it_value) || !TIMESPEC_VALID(&value->it_interval)) {
			errno = EINVAL;
			return -1;
		}
		n.nsec = timespec2nsec(&value->it_value);
		n.interval_nsec = timespec2nsec(&value->it_interval);
	}
	if((ret = TimerSettime(timerid, flags, value ? &n : 0, ovalue ? &o : 0)) != -1) {
		if(ovalue) {
			_timer_round_interval(&o.interval_nsec);
			_timer_round_interval(&o.nsec);
			nsec2timespec(&ovalue->it_value, o.nsec);
			nsec2timespec(&ovalue->it_interval, o.interval_nsec);
		}
	}
	return ret;	*/
}

int timer_delete( timer_t timerid ){
	TimerDestroy(timerid);
}

int MsgSend( int coid, const void* smsg, int sbytes, void* rmsg, int rbytes ){
	iov_t						iov_s;
	iov_t						iov_r;

	SETIOV(&iov_s, smsg, sbytes );
	SETIOV(&iov_r, rmsg, rbytes);

	return MsgSendv( coid, &iov_s,  1, &iov_r, 1 );
}

int MsgReceive( int chid, void * msg, int bytes, MSG_PAYLOAD_ITEM * info ){
	iov_t						iov_r;
	SETIOV(&iov_r, msg, bytes);

	return MsgReceivev( chid, &iov_r, 1, info );
}

int MsgReply( int rcvid, int status, const void* msg, int size )
{
	iov_t						iov_r;
	SETIOV(&iov_r, msg, size);

	return MsgReplyv( rcvid, status, &iov_r, 1 );
}

int MsgSendv( int coid, const iov_t* siov,  int sparts, const iov_t* riov, int rparts ) //	int MsgSend(int __coid, const void *__smsg, void *__rmsg)
{
	return syscall(SYSCALL_MSG_SENDV, coid, (long)siov, (long)sparts, (long)riov, (long)rparts);

	//	return 1;
	/*	MSG_DATA* pReplyMsg = (MSG_DATA*)__rmsg;
	int __sbytes		= sizeof(MSG_DATA);
	iMsgRcvLen = 0;
	syscall(SYSCALL_MSG_SENDV, __coid, (long)__smsg, __sbytes, (long)__rmsg, (long)&iMsgRcvLen);
	while(iMsgRcvLen==0){
		;
	}
	return pReplyMsg->iMsgLen;	*/
}

int MsgReceivev( int chid, const iov_t * riov, int rparts, MSG_PAYLOAD_ITEM * info )
{
	return syscall(SYSCALL_MSG_RECVV, (long)chid, (long)riov, (long)rparts, (long)info, (long)0);
}


int MsgReplyv( int rcvid, int status, const iov_t* riov, int rparts )	//	int MsgReply(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info){
{
	return syscall(SYSCALL_MSG_REPLYV, rcvid, status, (long)riov, (long)rparts, (long)0);
}

int MsgError( int rcvid, int status)
{
	return syscall(SYSCALL_MSG_REPLYV, rcvid, status, (long)0, (long)0, (long)0);
}

int MsgSendPulse ( int coid, int priority, int code, int value )	//	int MsgSendPulse(int __coid, const void *__smsg, void *__rmsg){
{
	return syscall(SYSCALL_MSG_SENDPULSE, (long)coid, (long)priority, (long)code, (long)value, (long)0 );	//	syscall(SYSCALL_MSG_SEND, __coid, __smsg, __sbytes, __rmsg, __rbytes);
}


int MsgDeliverEvent( int rcvid,  const struct sigevent* event ){
	return syscall(SYSCALL_MSG_DELIVER_EVENT, (long)event->coid, (long)event->priority, (long)event->code, (long)event->value, (long)rcvid );
}


int MsgReceivePulse( int chid, void * pulse, int bytes, MSG_PAYLOAD_ITEM * info )	//	int MsgRecvPulse(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info, int reserved){
{
	return syscall(SYSCALL_MSG_RECVPULSE, (long)chid, (long)pulse, (long)bytes, (long)info, (long)0);
}


int ChannelCreate( unsigned int flags ){
	return syscall(SYSCALL_CREATE_CHANNEL, (long)flags, (long)0, (long)0, (long)0, (long)0);
}

int ConnectAttach( uint32_t nd, pid_t pid, int chid, unsigned index, int flags ){
	return syscall(SYSCALL_CONNECT_ATTACH, (long)nd, (long)pid, (long)chid, (long)index, (long)flags);
}

int ChannelDestroy( int chid ){
	return syscall(SYSCALL_DESTROY_CHANNEL, (long)chid, (long)0, (long)0, (long)0, (long)0);
}

int ConnectDetach( int coid ){
	return syscall(SYSCALL_CONNECT_DETACH, (long)coid, (long)0, (long)0, (long)0, (long)0);
}


int ChannelCreateWithPath( unsigned int flags, char* pszPath ){
	return syscall(SYSCALL_CREATE_CHANNEL, (long)flags, (long)pszPath, (long)0, (long)0, (long)0);
}

int PathmgrLink(char* path, int type, handle_t handle)
{
	if (!path || !path[0])
		return ERR_INVALID_ARG;
	if (type < PATH_OBJECT_NONE || type >= PATH_OBJECT_MAXTYPE)
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHMGR_LINK, (long)path, (long)type, (long)handle, (long)0, (long)0);
}

int PathmgrUnlink(char* path)
{
	if (!path || !path[0])
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHMGR_UNLINK, (long)path, (long)0, (long)0, (long)0, (long)0);
}

int PathmgrRequire(char* path, path_object_t* p_obj)
{
	if (!path || !path[0])
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHMGR_REQ, (long)path, (long)p_obj, (long)0, (long)0, (long)0);
}

int PathobjOpen(char* path)
{
	if (!path || !path[0])
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHOBJ_OPEN, (long)path, (long)0, (long)0, (long)0, (long)0);
}

int PathobjClose(int fd)
{
	if (fd < 0)
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHOBJ_CLOSE, (long)fd, (long)0, (long)0, (long)0, (long)0);
}

int PathobjCtl(int fd, const void* param, size_t size)
{
	if (fd < 0)
		return ERR_INVALID_ARG;
	return syscall(SYSCALL_PATHOBJ_CTRL, (long)fd, (long)param, (long)size, (long)0, (long)0);
}

//int pathmgr_require(char* path, int* piPid, int* piChid){
//	return syscall(SYSCALL_PATHMGR_REQ, (long)path, (long)piPid, (long)piChid, (long)0, (long)0);
//}

int open_org(const char *pathname, const char *mode){
	return syscall(SYSCALL_NR_OPENAT, (long)pathname, (long)mode, 0, 0, 0);
}

int read_org(int fd, void *buffer, size_t nbytes){

	//	MSG_DATA 	msgSend;
	msgSend.iMsgType = MSG_READ;
	msgSend.iMsgLen  = nbytes;
	memset(msgSend.szMsg, 0, sizeof(msgSend.szMsg) );	//strcpy(msgSend.szMsg, buffer);	//	msgSend.pszMsg	 = buffer;

	//	MSG_DATA 	msgRecv;
	msgRecv.iMsgType = MSG_READ;
	msgRecv.iMsgLen  = nbytes;
	memset(msgRecv.szMsg, 0, sizeof(msgRecv.szMsg) );

	msgRecv.szMsg[0] = '#';
	msgRecv.szMsg[1] = '#';
	msgRecv.szMsg[2] = '#';
	msgRecv.szMsg[3] = '#';
	msgRecv.szMsg[4] = '#';
	msgRecv.szMsg[5] = '#';
	msgRecv.szMsg[6] = '#';

	//	msgRecv.pszMsg	 = buffer;
	//	int			iMsgRcvLen = 0xffff;

	devMsgSendC(fd, &msgSend, &msgRecv);	//	devMsgSend(fd, &msgSend, sizeof(msgSend), &msgRecv, &iMsgRcvLen);
	strcpy(buffer, (const char*)(msgRecv.szMsg));
	return msgRecv.iMsgLen;	//	iMsgType;
}

int write_org(int fd, const void *buffer, size_t nbytes){

	//	MSG_DATA 	msgSend;
	msgSend.iMsgType = MSG_WRITE;
	msgSend.iMsgLen  = nbytes;
	//	msgSend.pszMsg	 = buffer;
	strcpy(msgSend.szMsg, buffer);

	//	MSG_DATA 	msgRecv;
	msgRecv.iMsgType = MSG_READ;
	msgRecv.iMsgLen  = nbytes;
	//msgRecv.pszMsg	 = buffer;
	//	int			iMsgRcvLen = 0;
	return devMsgSendC(fd, &msgSend, &msgRecv);	//	return devMsgSend(fd, &msgSend, sizeof(msgSend), &msgRecv, &iMsgRcvLen);
}

int close_org(int fd){
	return syscall(SYSCALL_NR_CLOSE, fd, 0, 0, 0, 0);
}


error_t scr_write(char szTmp)	//	const char *buf, size_t len, int x, int y)
{
    return syscall(SYSCALL_NR_WRITESCREEN, szTmp, 0, 0, 0, 0);	//	(uintptr_t) buf, len, (long)x, (long)y, 0);
}



int SyncTypeCreate(unsigned long __type, sem_t* semp, const struct _sync_attr *__attr){
	
	return syscall(SYSCALL_SEMAPHORE_CREATE, (long)__type, (long)semp, (long)__attr, (long)0, (long)0);
 }

int SyncTypeTake(sem_t* semp){
	return syscall(SYSCALL_SEMAPHORE_TAKE, (long)semp, (long)0, (long)0, (long)0, (long)0);
 }

 int SyncSemPost(sem_t* semp){
	return syscall(SYSCALL_SEMAPHORE_POST, (long)semp, (long)0, (long)0, (long)0, (long)0);
 }

int SyncDestroy(sem_t* semp){
	return syscall(SYSCALL_SEMAPHORE_DESTROY, (long)semp, (long)0, (long)0, (long)0, (long)0);
}

int SyncGetValue(sem_t* semp){
	return syscall(SYSCALL_SEMAPHORE_SYNCVALUE, (long)semp, (long)0, (long)0, (long)0, (long)0);
}

 sem_t *semaphoreOpen(const char *name, int size,int oflag, ...) {
	va_list			ap;
	short			mode;
	unsigned		value;
	sem_t* ret_sem;

	xprintf("######[%s:%d] name Size:%d\n",__func__,__LINE__, size);

	va_start(ap, oflag);
	if (oflag & O_CREAT) {
		mode = va_arg(ap, short);
		value = va_arg(ap, unsigned int);
	}
	else {
		mode = 0;
		value = 0;
	}
	va_end(ap);
	//return _nsem_open(name, (oflag & ~O_ACCMODE) | O_RDWR, mode, value);
	oflag &= ~O_ACCMODE;
	oflag |= O_RDWR;
	ret_sem =(sem_t*)syscall(SYSCALL_SEMAPHORE_OPEN, (long)name, (long)oflag, (long)mode, (long)value, (long)size);
	return ret_sem;
}

sem_t* semaphoreClose(const char *name, int size,int oflag, ...) {
	va_list			ap;
	short			mode;
	unsigned		value;
	sem_t* ret_sem;

	xprintf("######[%s:%d] name Size:%d\n",__func__,__LINE__, size);

	va_start(ap, oflag);
	if (oflag & O_CREAT) {
		mode = va_arg(ap, short);
		value = va_arg(ap, unsigned int);
	}
	else {
		mode = 0;
		value = 0;
	}
	va_end(ap);
	//return _nsem_open(name, (oflag & ~O_ACCMODE) | O_RDWR, mode, value);
	oflag &= ~O_ACCMODE;
	oflag |= O_RDWR;
	ret_sem =(sem_t*)syscall(SYSCALL_SEMAPHORE_CLOSE, (long)name, (long)oflag, (long)mode, (long)value, (long)size);
	return ret_sem;
}


void screen_printf(char* pszTmp){
	while((*pszTmp)!='\0'){
		scr_write(*pszTmp);	//	x64_screen_printchar(*pszTmp);
		pszTmp++;
	}
	return;
}

pid_t getpid( void ){
	return g_procEnv.pid;
}

//	Set empty functions as discussed
int ThreadCtl( int cmd, void * data ){
	return syscall(SYSCALL_THREAD_CTL, (long)cmd, (long)data, (long)0, (long)0, (long)0);
}

//	Set empty functions as discussed
int getprio( pid_t pid ){
	return syscall(SYSCALL_THREAD_GETPRIORITY, (long)pid, (long)0, (long)0, (long)0, (long)0);		//	return 10;
}
//	Set empty functions as discussed
int setprio( pid_t pid, int prio ){
	return syscall(SYSCALL_THREAD_SETPRIORITY, (long)pid, (long)prio, (long)0, (long)0, (long)0);//	return 10;
}


int getprio_critical( pid_t pid ){
	return syscall(SYSCALL_THREAD_GETCRITICAL, (long)pid, (long)0, (long)0, (long)0, (long)0);		//	return 10;
}
int setprio_critical( pid_t pid, int prio ){
	return syscall(SYSCALL_THREAD_SETCRITICAL, (long)pid, (long)prio, (long)0, (long)0, (long)0);//	return 10;
}

int getprio_blocking( pid_t pid ){
	return syscall(SYSCALL_THREAD_GETBLOCKING, (long)pid, (long)0, (long)0, (long)0, (long)0);		//	return 10;
}
int setprio_blocking( pid_t pid, int prio ){
	return syscall(SYSCALL_THREAD_SETBLOCKING, (long)pid, (long)prio, (long)0, (long)0, (long)0);//	return 10;
}

int GetSysInfo(sysinfo_type_e type, uintptr_t param, void* p_sysinfo, size_t size)
{
	return syscall(SYSCALL_GET_SYSINFO, (long)type, (long)param, (long)p_sysinfo, (long)size, (long)0);
}
