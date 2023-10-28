#ifndef __KIRK_TYPES_H__
#define __KIRK_TYPES_H__


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdalign.h>
#include <stdarg.h>
#include <return_code.h>

#include "temp_header.h"

typedef unsigned long phy_sigset_t;

typedef void (*phy_sighandler_t)(int signo);

#define ALIGN_DOWN(value, align) ((size_t)(value) & ~(((size_t)(align)) -1))
#define ALIGN_UP(value, align)   ALIGN_DOWN(((size_t)(value)) + ((size_t)(align)-1), (size_t)(align))
#define IS_ALIGNED(value, align) (((value) & ((align) -1)) == 0)

#define STATIC_ASSERT(expr)      _Static_assert(expr, #expr);


#define MAX(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) __a = (a);                                               \
        __typeof__(b) __b = (b);                                               \
        (__a > __b) ? __a : __b;                                               \
    })
#define MIN(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) __a = (a);                                               \
        __typeof__(b) __b = (b);                                               \
        (__a < __b) ? __a : __b;                                               \
    })
#define OK_OR_RETURN(expr)                                                     \
    ({                                                                         \
        error_t any_or_err = (expr);                                           \
        if (IS_ERROR(any_or_err)) {                                            \
            return any_or_err;                                                 \
        }                                                                      \
        any_or_err;                                                            \
    })

#define DIV_ROUNDUP(A, B) ({ \
    typeof(A) _a_ = A;       \
    typeof(B) _b_ = B;       \
    (_a_ + (_b_ - 1)) / _b_; \
})

typedef uint32_t	id_t;
typedef id_t	    pid_t;
typedef id_t	    tid_t;
#define NULL_ID     ((id_t)-1L)

typedef uintptr_t	handle_t;
#define NULL_HANDLE ((handle_t)0L)

/*form core_ca7.h*/
#define     __I     volatile const           /*!< defines 'read only' permissions */
#define     __O     volatile                 /*!< defines 'write only' permissions*/
#define     __IO    volatile                 /*!< defines 'read / write' permissions*/

/* form core_ca7.h, following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */
	



#endif


