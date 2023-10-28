#pragma once

#include "types.h"
#include "list.h"
#include "print_macros.h"

#include "ipc.h"
#include <libc/include/iomsg.h>


#define sig_mask(sig_no)    (1u << sig_no)

union sigval {
    int sival_int;
    void *sival_ptr;
};

typedef struct {
     int si_signo;
     int si_code;
     union sigval si_value;
     int si_errno;
     pid_t si_pid;
     short si_uid;
     void *si_addr;
     int si_status;
     int si_band;
 } siginfo_t;

 struct siginfo_node
{
    siginfo_t si;
    list_t list;
};

static void _signal_deliver(THREAD_t* tid);
void phy_thread_handle_sig(bool clean_state);

static void _signal_default_handler(int signo)
{
    printf("handled signo[%d] with default action.", signo);
    return ;
}


#define PHY_THREAD_STAT_SIGNAL           0x10     /**< task hold signals */
#define PHY_THREAD_STAT_SIGNAL_READY     (PHY_THREAD_STAT_SIGNAL | PHY_THREAD_READY)
#define PHY_THREAD_STAT_SIGNAL_WAIT      0x20     /**< task is waiting for signals */
#define PHY_THREAD_STAT_SIGNAL_PENDING   0x40     /**< signals is held and it has not been procressed */
#define PHY_THREAD_STAT_SIGNAL_MASK      0xf0

typedef void (*phy_sighandler_t)(int signo);

/* Fake signal functions.  */
#define	SIG_ERR	 ((phy_sighandler_t) -1)	/* Error return.  */
#define	SIG_DFL	 ((phy_sighandler_t)  0)	/* Default action.  */
#define	SIG_IGN	 ((phy_sighandler_t)  1)	/* Ignore signal.  */






/* RT-Thread error code definitions */
#define PHY_EOK                          0               /**< There is no error */
#define PHY_ERROR                        1               /**< A generic error happens */
#define PHY_ETIMEOUT                     2               /**< Timed out */
#define PHY_EFULL                        3               /**< The resource is full */
#define PHY_EEMPTY                       4               /**< The resource is empty */
#define PHY_ENOMEM                       5               /**< No memory */
#define PHY_ENOSYS                       6               /**< No system */
#define PHY_EBUSY                        7               /**< Busy */
#define PHY_EIO                          8               /**< IO error */
#define PHY_EINTR                        9               /**< Interrupted system call */
#define PHY_EINVAL                       10              /**< Invalid argument */
#define PHY_SIG_MAX          32
#define sig_valid(sig_no)   (sig_no >= 0 && sig_no < PHY_SIG_MAX)

/* Values for `si_code'.  Positive values are reserved for kernel-generated
   signals.  */
enum
{
  SI_ASYNCNL = -60,		/* Sent by asynch name lookup completion.  */
  SI_DETHREAD = -7,		/* Sent by execve killing subsidiary
				   threads.  */
  SI_TKILL,			/* Sent by tkill.  */
  SI_SIGIO,			/* Sent by queued SIGIO. */
#if __SI_ASYNCIO_AFTER_SIGIO
  SI_ASYNCIO,			/* Sent by AIO completion.  */
  SI_MESGQ,			/* Sent by real time mesq state change.  */
  SI_TIMER,			/* Sent by timer expiration.  */
#else
  SI_MESGQ,
  SI_TIMER,
  SI_ASYNCIO,
#endif
  SI_QUEUE,			/* Sent by sigqueue.  */
  SI_USER,			/* Sent by kill, sigsend.  */
  SI_KERNEL = 0x80		/* Send by kernel.  */

#define SI_ASYNCNL	SI_ASYNCNL
#define SI_DETHREAD	SI_DETHREAD
#define SI_TKILL	SI_TKILL
#define SI_SIGIO	SI_SIGIO
#define SI_ASYNCIO	SI_ASYNCIO
#define SI_MESGQ	SI_MESGQ
#define SI_TIMER	SI_TIMER
#define SI_ASYNCIO	SI_ASYNCIO
#define SI_QUEUE	SI_QUEUE
#define SI_USER		SI_USER
#define SI_KERNEL	SI_KERNEL
};

typedef siginfo_t phy_siginfo_t;
