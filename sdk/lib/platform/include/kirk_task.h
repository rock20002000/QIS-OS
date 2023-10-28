#ifndef __KIRK_TASK_H__
#define __KIRK_TASK_H__

#include <types.h>
#include <platform/include/mmap.h>
#include <platform/include/msg.h>
#include <platform/include/resmgr.h>

 
#define THREAD_STATE_MASK             0x07
#define THREAD_STATE_YIELD_MASK       0x08
#define THREAD_STATE_SIGNAL_MASK      0xf0

typedef enum{
	THREAD_STATE_IDLE = 0,
	THREAD_STATE_BLOCKED=2,  //SUSPEND
	THREAD_STATE_READY=1,
	THREAD_STATE_RUNNING=3,
	THREAD_STATE_WAITING=5,
	THREAD_STATE_EXIT=4,
	//THREAD_STATE_YIELD=0x8,
	//////////////////////////////////
	THREAD_STATE_SIGWAITINFO,
	THREAD_STATE_SIGWAIT_INTREVENT,
	//THREAD_STATE_SIGWAIT_YIELD,
	THREAD_STATE_NANOSLEEP,
	THREAD_STATE_SIGNAL=0x10,           /**< task hold signals */
	THREAD_STATE_SIGNAL_READY = (THREAD_STATE_SIGNAL | THREAD_STATE_READY ),
	THREAD_STATE_SIGNAL_WAIT =  0x20,   /**< task is waiting for signals */
	THREAD_STATE_SIGNAL_PENDING =   0x40  /**< signals is held and it has not been procressed */

} EM_THREAD_STATE;

typedef enum{
    PROCESS_STATE_NORMAL,
    PROCESS_STATE_EXIT,
    PROCESS_STATE_ZOMBIE
}EM_PROCESS_STATE;

#ifndef MAIN_STACK_SIZE
 #define MAIN_STACK_SIZE	(0x0000F000)
#endif

// process_local_storage
#ifndef PROC_NAME_SIZE
 #define PROC_NAME_SIZE	64  /* CONFIG_PROC_NAME_LEN */
#endif
typedef struct _process_env{
	uint32_t	size;
	pid_t		pid;
	vaddr_t		vdso_data;
	vaddr_t		vdso_text;
	size_t		cycles_per_sec;
	size_t		systimer_freq;
} PROCESS_ENV;
#define PROCESS_ENV_SIZE			sizeof(PROCESS_ENV)
#define PROC_ENV_ADDR(rodata_end)	((vaddr_t)(rodata_end) - PROCESS_ENV_SIZE)

typedef struct
{
	uintptr_t		entry;
	uint32_t		nums;
	map_params_t	params[1];
} proc_map_attr_t;

typedef struct
{
	pid_t			parent;
	int				stat;	// main thread start status
	char			name[PROC_NAME_SIZE];
	proc_map_attr_t	map;	// must be last member
} *p_proc_create_attr_t;

#define PROC_CMDLINE_SIZE				(2048)
#define PROC_CMDLINE_PTR(env)			((uintptr_t)(env) + PAGE_SIZE - PROC_CMDLINE_SIZE)
#define PROC_CMDLINE_ADDR(rodata_end)	PROC_CMDLINE_PTR(PROC_ENV_ADDR(rodata_end))

typedef struct _thread_env{
	tid_t pid;
} THREAD_ENV;

typedef enum {
    SYSINFO_VERSION,
    SYSINFO_PROCESS,
    SYSINFO_MEMORY,
    SYSINFO_SYSPAGE,
    SYSINFO_MAX
} sysinfo_type_e;

typedef struct
{
	char		name[PROC_NAME_SIZE];
	pid_t		pid;
	pid_t		ppid;
	tid_t		tid;
	size_t		vsz;
	size_t		rss;
	uint32_t	code;
	uint32_t	data;
	uint32_t	vstack;
	uint32_t	stack;
	int			stat;
	int			priority;
	int			threads;
	size_t		start;
	size_t		time;
} ps_info_t;

 //typedef struct __pthread_attr_s
 //{
 //  int __detachstate;
 //  int __schedpolicy;
 //  struct __sched_param __schedparam;
 //  int __inheritsched;
 //  int __scope;
 //  size_t __guardsize;
 //  int __stackaddr_set;
 //  void *stackaddr;
 //  size_t __stacksize;
 //} pthread_attr_t;

typedef struct __pthread_attr_s
{
	uint32_t iStackSize;
} pthread_attr_t;

struct spinlock_user {
     uint32_t lock;
     //	uint32_t cpu;
     //	void * blob;
};

typedef struct spinlock_user pthread_spinlock_t;

typedef struct _mutex{
	int __m_reserved;               /* Reserved for future use */
	int __m_count;                  /* Depth of recursive locking */
	pthread_spinlock_t			wait_lock;
} pthread_mutex_t;

typedef struct
{
	int __mutexkind;
} pthread_mutexattr_t;

typedef size_t pthread_t;	//	typedef unsigned long int pthread_t;
typedef void(*start_rtn)(void);
typedef void(*start_rtn_withParam)(void*);
typedef size_t int_handler_q(int irqNum,void* dev_id, void* irqdata);

#include	<platform/include/thread_schedule.h>
#endif
