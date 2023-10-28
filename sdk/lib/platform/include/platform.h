#ifndef __KIRK_PLATFORM_H__
#define __KIRK_PLATFORM_H__

#include <libc/include/stdio.h>
#include <libc/include/string.h>
#include <platform/include/mmap.h>
#include <platform/include/msg.h>
#include <platform/include/kirk_task.h>
#include <platform/include/pathobj.h>
#include <platform/include/resmgr.h>
#include <platform/include/syscalls.h>
#include <platform/include/time.h>
#include <arch_common.h>

void xprintf(const char *fmt, ...);
#define printf_s   xprintf

#endif
