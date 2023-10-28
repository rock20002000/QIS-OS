#ifndef __PRINT_MACROS_H__
#define __PRINT_MACROS_H__

#include <types.h>
#include <libc/include/stdio.h>


#ifndef KIRK_APP
#    define printk printf
int printf(const char *fmt, ...);
#else
#    define printk xprintf
void xprintf(const char *fmt, ...);
#endif


// ANSI escape sequences (SGR).
#define SGR_ERR      "\e[1;91m"  // Bold red.
#define SGR_WARN     "\e[0;33m"  // Yellow.
#define SGR_WARN_DBG "\e[1;33m"  // Bold yellow.
#define SGR_INFO     "\e[0;96m"  // Cyan.
#define SGR_DEBUG    "\e[1;32m"  // Bold green.
#define SGR_RESET    "\e[0m"

#ifdef USING_DEBUG
#define DBG(fmt, ...)                                                          \
    do {                                                                       \
        printf(SGR_DEBUG "[%s]:%d \n" fmt SGR_RESET "\n",__FILE__, __LINE__,     \
               ##__VA_ARGS__);                                                 \
    } while (0)
    
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf(SGR_ERR "PANIC: [%s]:%d\n " fmt SGR_RESET "\n",__FILE__, __LINE__,    \
               ##__VA_ARGS__);                                                  \
        while (1);                                                              \
        __builtin_unreachable();                                                \
    } while (0)

#define WARN(fmt, ...)                                                        \
    do {                                                                       \
        printf(SGR_ERR "WARN: [%s]:%d\n " fmt SGR_RESET "\n",__FILE__, __LINE__,    \
               ##__VA_ARGS__);                                                 \
    } while (0)

#define ASSERT(expr)                                                           \
    do {                                                                       \
        if (!(expr)) {                                                         \
            printf(SGR_ERR "ERROR: [%s]:%d: ASSERTION FAILURE: %s " SGR_RESET "\n", \
                   __FILE__, __LINE__, #expr);               \
            for(;;);                                                             \
            __builtin_unreachable();                                           \
        }                                                                      \
    } while (0)

#define KDBG(fmt, ...)  \
    do { printk(SGR_DEBUG fmt SGR_RESET, ##__VA_ARGS__); } while (0)
#define KWARN(fmt, ...) \
    do { printk(SGR_ERR fmt SGR_RESET, ##__VA_ARGS__); } while (0)
#define PDBG(fmt, ...)  \
    do { printf(SGR_DEBUG fmt SGR_RESET, ##__VA_ARGS__); } while (0)
#define PWARN(fmt, ...) \
    do { printf(SGR_ERR fmt SGR_RESET, ##__VA_ARGS__); } while (0)
#define PINFO(fmt, ...) \
    do { printf(SGR_INFO fmt SGR_RESET, ##__VA_ARGS__); } while (0)
#else
#define DBG(fmt, ...)  
#define PANIC(fmt, ...) 
#define WARN(fmt, ...) 
#define ASSERT(expr)
#define KDBG(fmt, ...)  
#define KWARN(fmt, ...)  
#define PDBG(fmt, ...)  
#define PWARN(fmt, ...)  
#define PINFO(fmt, ...)  
#endif

#define DEBUG_ASSERT(expr) ASSERT(expr)

#define KINFO(fmt, ...) \
    do { printk(SGR_INFO fmt SGR_RESET, ##__VA_ARGS__); } while (0)


#endif
