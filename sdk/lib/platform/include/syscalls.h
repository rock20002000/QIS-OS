#ifndef __LIB_SYSCALL_H__
#define __LIB_SYSCALL_H__

#include <types.h>
#include <platform/include/syscall_vec.h>
#include <platform/include/time.h>
#include <platform/include/kirk_task.h>
#include <platform/include/resmgr.h>
#include <platform/include/pathobj.h>
#include <arch_syscall.h>


//syscall ID 1:syswrite
error_t sys_write(const char *buf, size_t len, void* fd);

//	void* user_int_register(int_handler_q* handler, size_t irqnum, void*  dev_id, void* irqdata, long flag, unsigned long offset);

void threadShellForIntr(int iIrq, start_rtn_withParam userFunction);	//	void threadShell(IRQ_HANDLER_PARAM* pIrqHandlerParam);	//	start_rtn threadFunction);

void threadShellWithExit(void* pThreadParamUser, start_rtn_withParam pFunctionUser);

int devRegister(int* pStatus, const char *pszFullPath, int devToken, DEVICE_FUNCTIONS* pDevFunctions);

int devMsgSendC(int __coid, const void *__smsg, void *__rmsg);	//	int devMsgSend(int __coid, const void *__smsg, int __sbytes, void *__rmsg, volatile int* __rbytes);

int devMsgRecvC(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info, int reserved);

int devMsgReplyC(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info);

int devThreadWait();

int TimerCreate( clockid_t id, const struct sigevent *event );
int timer_create( clockid_t clock_id, struct sigevent * evp, timer_t * timerid );
int TimerDestroy( timer_t id );
//int TimerInfo( pid_t pid, timer_t id, int flags,  struct _timer_info* info );
int TimerSettime( timer_t id,  int flags,  const struct _itimer * itime, struct _itimer * oitime );
int timer_settime( timer_t timerid,  int flags,  const struct itimerspec * value,  struct itimerspec * ovalue );
int timer_delete( timer_t timerid );

int MsgSend( int coid, const void* smsg, int sbytes, void* rmsg, int rbytes );
int MsgReceive( int chid, void * msg, int bytes, MSG_PAYLOAD_ITEM * info );
int MsgReply( int rcvid, int status, const void* msg, int size );

int MsgSendv( int coid, const iov_t* siov,  int sparts, const iov_t* riov, int rparts );
int MsgReceivev( int chid, const iov_t * riov, int rparts, MSG_PAYLOAD_ITEM * info );
int MsgReplyv( int rcvid, int status, const iov_t* riov, int rparts );

int MsgError( int rcvid, int status);

int MsgSendPulse ( int coid, int priority, int code, int value );
int MsgReceivePulse( int chid, void * pulse, int bytes, MSG_PAYLOAD_ITEM * info );

int MsgDeliverEvent( int rcvid,  const struct sigevent* event );

int open_org(const char *pathname, const char *mode);

int read_org(int fd, void *buffer, size_t nbytes);

int write_org(int fd, const void *buffer, size_t nbytes);

int close_org(int fd);

error_t scr_write(char szTmp);

void screen_printf(char* pszTmp);

void*  AttachInterruptC(int iIrq, start_rtn userFunction);	//	void* AttachInterruptC(IRQ_HANDLER_PARAM* pIrqHandlerParam);

int InterruptAttach( int intr, const struct sigevent * (* handler)(void *, int), const void * area, int size, unsigned flags );
int InterruptDetach( int id );

int InterruptAttachEvent(int __intr, const struct sigevent *__event, unsigned __flags);
int InterruptDetachEvent( int __intr);	//	, const struct sigevent *__event );

int InterruptMask( int intr, int id );

int InterruptUnmask( int intr, int id );

int clock_gettime( clockid_t clock_id, struct timespec * tp);

int kernel_nop();

//static void*  __attribute__((noinline)) AttachInterruptTest(start_rtn handler){
//	return (void*)syscall(SYSCALL_THREAD_CREATE, (long)threadShell, NULL, NULL, (long)handler, 0);
//}

//typedef unsigned long int pthread_t;
//typedef void*(*start_rtn)(void*);
//#define SYSCALL_THREAD_CREATE       52
int pthread_create(pthread_t*  thread_id, void* threadAttrs, start_rtn_withParam handler, void* threadParams);
int pthread_detach( pthread_t thread );
void pthread_exit( void* value_ptr );

int pthread_join( pthread_t thread,  void** value_ptr );

long int InterruptWait(int iIrqNo, int* piTimeOut);	//	long int InterruptWait(int iIrqNo);

long int waitInterruptEvent(int iIrqNo);

long int notifyInterruptEvent(int iIrqNo);

long int yield();

void msleep(int iMs);
void sleep(int iSec);

long int scheduleToRun(int iThreadID);

long int init_process_main_thread(const p_proc_create_attr_t pca);

int  shell_exec_id(int iProcessIndex);

int  exec(char* pszCmdLine);

int  shell_exec(char* pszCmdLine);

void exit( int status );

extern const PROCESS_ENV g_procEnv;

pid_t getpid( void );

//	Create a communications channel
int ChannelCreate( unsigned flags );
//	Establish a connection between a process and a channel
int ConnectAttach( uint32_t nd, pid_t pid, int chid, unsigned index, int flags );
//	Destroy a communications channel
int ChannelDestroy( int chid );
//	Break a connection between a process and a channel
int ConnectDetach( int coid );

int ThreadCtl( int cmd, void * data );

int getprio( pid_t pid );
int setprio( pid_t pid, int prio );

int getprio_critical( pid_t pid );
int setprio_critical( pid_t pid, int prio );

int getprio_blocking( pid_t pid );
int setprio_blocking( pid_t pid, int prio );


//////////////////////////////////////////////////////////////////////

int ChannelCreateWithPath( unsigned flags, char* pszDevPath );

int PathmgrLink(char* path, int type, handle_t handle);
int PathmgrUnlink(char* path);
int PathmgrRequire(char* path, path_object_t* p_obj);
int PathobjOpen(char* path);
int PathobjClose(int fd);
int PathobjCtl(int fd, const void* param, size_t size);

/**
    @brief get system info
    @param type sysinfo type
    @param param sysinfo parameter
    @param p_sysinfo sysinfo output pointer
    @param size sysinfo buffer size
    @return > 0 is sysinfo size, < 0 is error code.
*/
int GetSysInfo(sysinfo_type_e type, uintptr_t param, void* p_sysinfo, size_t size);

#endif
