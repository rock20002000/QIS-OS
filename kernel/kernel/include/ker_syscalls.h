#ifndef __K_SYSCALLS_H__
#define __K_SYSCALLS_H__

#include "mm.h"

//long handle_syscall(int n, long a1, long a2, long a3, long a4, long a5);

#ifndef USER_TEXT_MAXSIZE
 #define USER_TEXT_MAXSIZE  (0x400)
#endif

static inline void* map_user_data(const void* user_ptr, size_t size)
{
	return map_from_user(NULL, user_ptr, size, KERNEL_MAP_FLAGS);
}
static inline void* map_user_rdata(const void* user_ptr, size_t size)
{
	return map_from_user(NULL, user_ptr, size, KERNEL_MAP_RFLAGS);
}
static inline error_t unmap_user_data(const void* ptr, size_t size)
{
    return kernel_unmap(ptr, size);
}
static inline void* map_user_text(const void* user_ptr, size_t size)
{
    if (!size)
    {
        size = USER_TEXT_MAXSIZE;
    }
	return map_from_user(NULL, user_ptr, size, KERNEL_MAP_XFLAGS);
}
static inline error_t unmap_user_text(const void* ptr, size_t size)
{
    if (!size)
    {
        size = USER_TEXT_MAXSIZE;
    }
    return kernel_unmap(ptr, size);
}

#endif
