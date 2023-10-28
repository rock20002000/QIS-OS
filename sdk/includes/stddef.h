#ifndef __STDDEF_H__
#define __STDDEF_H__

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WCHAR_TYPE__ wchar_t;

#ifdef NULL
#undef NULL
#endif

#define NULL ((void *)0)

#ifndef offsetof
 #define offsetof(type,field)    __builtin_offsetof(type, field)
#endif
#ifndef is_constant
 #define is_constant(expr)       __builtin_constant_p(expr)
#endif

#ifndef __unused
 #define __unused                __attribute__((unused))
#endif
#ifndef __packed
 #define __packed                __attribute__((packed))
#endif
#ifndef __noreturn
 #define __noreturn              __attribute__((noreturn))
#endif
#ifndef __weak
 #define __weak                  __attribute__((weak))
#endif
#ifndef __mustuse
 #define __mustuse               __attribute__((warn_unused_result))
#endif
#ifndef __aligned
 #define __aligned(aligned_to)   __attribute__((aligned(aligned_to)))
#endif

#endif
