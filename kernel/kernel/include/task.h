#ifndef __SYS_TASK_H
#define __SYS_TASK_H

#include <arch.h>
#include <boot/include/bootinfo.h>
#include <platform/include/platform.h>
#include <utils/include/dyn_array.h>
#include <utils/include/id_pool.h>
#include <utils/include/list.h>

#include <platform/include/thread_privilege.h>

#include "interrupt.h"
#include "locks.h"
#include "mm.h"
#include "ker_time.h"


typedef size_t pthread_t;	//	typedef unsigned long int pthread_t;

#define TASK_HEAP_SIZE (16 * 1024 * 1024)  // 16M

#define MAX_DEVICE_TYPE_SUPPORT 1

#define MAX_THREAD_PER_PROC 256

#define MAX_PRIORITYS 256  //	255

#define MAX_PROCESS_SUPPORT 65536

#define MAX_RESOURCE_DEVICE_SUPPORT 1  //	65536

#define MAX_INTERRUPT_SUPPORT 256

#define MAX_SLEEP_TYPE_SUPPORT 256

//	#define MAX_CHANNEL_SUPPORT 		65536

#define MAX_SYNC_SUPPORT 65536

#ifndef PROC_NAME_SIZE
 #define PROC_NAME_SIZE 64  /* CONFIG_PROC_NAME_LEN */
#endif
#define CONFIG_PROC_NAME_LEN PROC_NAME_SIZE
#define CONFIG_THREAD_NAME_LEN 32

struct PROCESS;
typedef struct PROCESS PROCESS_t;

typedef struct _tag_thread_settings{
	int fIOAccess;
}THREAD_SETTINGS;

typedef struct tagThread{
  struct phy_object obj;
  list_t thdList; /**< the thread list */
  /// The arch-specific fields.(Usually contain sp and stack in kernel&user
  /// space)
  struct arch_thread arch;
  /// The task ID. Starts with 1.
  size_t tid;
  /// The name of task terminated by NUL.
  char name[CONFIG_THREAD_NAME_LEN];
  // pointer to its process
  PROCESS_t* process;
  iframe_t regs;
  SCHEDULE_POLICY	schedulePolicy;
  THREAD_SETTINGS	thread_settings;

  /// The task priority. The lower value means higher priority. The scheduler
  /// always picks the runnable task with the highest priority. If there're
  /// multiple runnable tasks with the same highest priority, the kernel
  /// schedules in round-robin fashion.
  uint32_t priority;

  /// The remaining time slice in ticks. If this value reaches 0, the kernel
  /// switches into the next task (so-called preemptive context switching).
  size_t quantum;
  EM_THREAD_STATE state;
  uint64_t STATE_BITMAP;
  uint32_t number_mask; 
  bool is_user_thread;

  /* error code */
  err_t error; /**< error code */
  Timer thread_timer;

    /* thread event */
    uint32_t event_set;
    uint8_t  event_info;

    /* stack point and entry */
    void       *sp;              /**< stack point */
    void       *entry;           /**< entry */
    void       *parameter;       /**< parameter */
    void       *stack_addr;      /**< stack address */
    uint32_t   stack_size;      /**< stack size */

    void    *sig_ret;     /**< the return stack pointer from signal */

    phy_sigset_t     sig_pending;                        /**< the pending signals */
    phy_sigset_t     sig_mask;                           /**< the mask bits of signal */

    phy_sighandler_t *sig_vectors;                       /**< vectors of signal handler */
    void            *si_list;                           /**< the signal infor list */

} THREAD_t;

struct arch_proc;
typedef struct PROCESS {
  /// The arch-specific fields.
  struct arch_proc arch;
  /// For MMU
  mm_space_t ms;
  mm_address_t addr;
  /// The process ID.
  pid_t pid;
  /// The name of task terminated by NUL.
  char name[CONFIG_PROC_NAME_LEN];

  DYNARRAY_STRUCT(THREAD_t*) threads;
  DYNARRAY_STRUCT(PROCESS_t*) children;

  PROCESS_t* parent;
  // PID of pager task (1 stands for proc,0 stands for process itself)
  uint64_t pager;

  // lock to protect this struct
  yieldlock_t lock;
  // ID pool for threads
  id_pool_t thread_id_pool;

  unsigned int flags;
  unsigned int __state;

  systime_t start_time;    // start ticks&cycles
  systime_t run_time;      // total used ticks&cycles
  systime_t run_start;     // last running start ticks&cycles

  const char* path;

  // PROCESS_ENV* env;
  void** fdtab;
  ////////////////////////////////////////////
} task_struct;

/*
typedef struct _mutex_id{
        uint32_t	nID;
        uint32_t	nReserved;
}pthread_mutex_t;
*/
typedef struct _mutex_desc {
  uint32_t nMutexID;  // pthread_mutex_t nMutexID;
  atomic_t count;     //	0: unlocked, positive values: locked, possible waiters
                      //count
  spinlock_t wait_lock;
  THREAD_t* pThreadMutexOwner;
  DYNARRAY_STRUCT(THREAD_t*)
  mutex_wait_queue_lookup_lists;  //	struct list_head	wait_list;
} pthread_mutex_desc;


typedef void thread_func();


#include "cpu/per_cpu.h"


/*
typedef enum{
    THREAD_STATE_IDLE,
    THREAD_STATE_BLOCKED,
    THREAD_STATE_READY,
    THREAD_STATE_RUNNING,
    THREAD_STATE_WAITING,
    THREAD_STATE_DEAD,
        //////////////////////////////////
        THREAD_STATE_SIGWAITINFO,
        THREAD_STATE_SIGWAIT_INTREVENT,
        THREAD_STATE_SIGWAIT_YIELD,
        THREAD_STATE_NANOSLEEP
}EM_THREAD_STATE;

typedef enum{
    PROCESS_STATE_NORMAL,
    PROCESS_STATE_EXIT,
    PROCESS_STATE_ZOMBIE
}EM_PROCESS_STATE;
*/


// In task/init_proc.c
void init_proc(void);
error_t load_proc(const p_proc_create_attr_t pca);

bool is_root_process(PROCESS_t* proc);
//static inline
const char* process_path(PROCESS_t* proc);
PROCESS_t* current_process(THREAD_t* current_thread);

//static inline bool is_root_process(PROCESS_t* proc) { return 0 == proc->pid; }
//static inline const char* process_path(PROCESS_t* proc) { return proc->path; }
//static inline PROCESS_t* current_process(THREAD_t* current_thread) {
//  if (!current_thread) current_thread = CURRENT_THREAD;
//  if (!current_thread) return NULL;
//  return current_thread->process;
//}

// In task/sched.c
void start_sched(void);
bool start_sched_intr(iframe_t* iframe, int fWithNormalQueue,
                      THREAD_t* pThreadNextToRun);
void start_sched_intr_privilege(THREAD_t* pPrivilegeThread, iframe_t* iframe);

void scheduler_init();
size_t process_lookuplist_push(PROCESS_t* proc);
PROCESS_t* lookupProcessWithPageTableAddress(size_t pageTableAddrToCheck);
void process_lookuplist_remove(PROCESS_t* proc);
PROCESS_t* getProcessWithPid(pid_t pidTmp);

THREAD_t* getThreadNextToRun(void);

size_t thread_fifo_runqueue_push(THREAD_t* thread);
size_t thread_fifo_runqueue_add_first(THREAD_t* thread);
THREAD_t* thread_fifo_runqueue_pop(void);
void thread_fifo_runqueue_remove(THREAD_t* thread);

size_t thread_fifo_waitqueue_push(THREAD_t* thread);
THREAD_t* thread_fifo_waitqueue_pop(void);
THREAD_t* thread_fifo_waitqueue_get(int iThreadID);
void thread_fifo_waitqueue_remove(THREAD_t* thread);

size_t thread_runqueue_push(THREAD_t* thread);
THREAD_t* thread_runqueue_pop(void);
void thread_runqueue_remove(THREAD_t* thread);
size_t thread_runqueue_add_first(THREAD_t* thread);

size_t thread_privilegequeue_push(int iPrLevel, THREAD_t* thread);
THREAD_t* thread_privilegequeue_pop(void);

size_t thread_waitqueue_push(THREAD_t* thread);
THREAD_t* thread_waitqueue_pop(void);
THREAD_t* thread_waitqueue_get(int iThreadID);
void thread_waitqueue_remove(THREAD_t* thread);

size_t thread_destroyqueue_push(THREAD_t* thread);

// In task/thread.c
THREAD_t* init_thread(THREAD_t* thread, const char* name, thread_func function,
                      uint8_t priority, bool is_user);
THREAD_t* init_thread_withParam(THREAD_t* thread, const char* name,
                                thread_func function, void* threadParams,
                                void* extParam, THREAD_t* pCurThread,
                                uint8_t priority, bool is_user);
THREAD_t* msg_pthread_create(PROCESS_t* proc, pthread_t* thread_id,
                             void* threadAttrs, start_rtn handler,
                             void* threadParams, int iStartStatus);
THREAD_t* msg_pthread_create_withParam(THREAD_t* pCurThread,
                                       pthread_t* thread_id, void* threadAttrs,
                                       start_rtn_withParam handler,
                                       void* threadParams, void* extParam,
                                       int iStartStatus);
size_t thread_stack_size(THREAD_t* thread);

/**
    @brief destroy thread
    @param thread thread struct pointer
    @param deattach remove from process or not
    @return None
*/
void destroy_thread(THREAD_t* thread, bool deattach);

// process.c
/**
    @brief create process
    @param name Process name
    @return  Process struct pointer
*/
PROCESS_t* process_create(const char* name);

/**
    @brief destroy process
    @param process Process struct pointer
    @return None
*/
void process_destroy(PROCESS_t* process);

void process_bind_thread(PROCESS_t* prp, THREAD_t* trp);
void process_unbind_thread(PROCESS_t* prp, THREAD_t* trp);

static inline bool is_active_process(PROCESS_t* proc)
{
    if (!proc)
        return false;
    return proc->__state == PROCESS_STATE_NORMAL;
}

#ifdef CONFIG_TASK_HEAP
/**
    @brief get process heap size
    @param proc Process struct pointer
    @return heap size
*/
size_t process_heap_size(PROCESS_t* proc);
#endif
/**
    @brief process enviroment initialize
    @param proc process struct pointer
    @return error code, 'EOK' is successful
*/
error_t process_init_env(PROCESS_t* proc);

/**
    @brief process set current path
    @param proc process struct pointer
    @param path path name string
    @return error code, 'EOK' is successful
*/
error_t process_set_path(PROCESS_t* proc, const char* path);

#ifndef PROCESS_FDMAX
#define PROCESS_FDMAX (0x2000)
#endif
/**
    @brief set pointer in process fd table
    @param proc process struct pointer
    @param int fd number, < 0 means new item
    @param ptr fd pointer
    @return fd number, < 0 is error code
*/
int fd_set(PROCESS_t* proc, int fd, void* ptr);

/**
    @brief get pointer in process fd table
    @param proc process struct pointer
    @param int fd number
    @return fd pointer
*/
void* fd_get(PROCESS_t* proc, int fd);

#ifndef USR_PID_START
 #define USR_PID_START  0x8000
#endif
/**
    @brief get pid for user
    @param proc process struct pointer
    @return pid
*/
pid_t get_user_pid(PROCESS_t* proc);

/**
    @brief get main thread
    @param proc process struct pointer
    @return main thread pointer
*/
THREAD_t* get_main_thread(PROCESS_t* proc);

/**
    @brief get system info
    @param type sysinfo type
    @param param sysinfo parameter
    @param p_sysinfo sysinfo output pointer
    @param size sysinfo buffer size
    @return > 0 is sysinfo size, < 0 is error code.
*/
error_t get_sysinfo(sysinfo_type_e type, uintptr_t param, void* p_sysinfo, size_t size);

// taskmgr.c
void init_taskmgr(syspage_t* syspage_ptr);

#endif
