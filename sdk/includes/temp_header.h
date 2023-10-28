#ifndef __TEMP_DEFINITIONS_H
#define __TEMP_DEFINITIONS_H
#include <stddef.h>
#include <stdint.h>
#include <gConfig.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 u8_t;
typedef u16 u16_t;
typedef u32 u32_t;
typedef u64 u64_t;



typedef int8_t __s8;
typedef uint8_t __u8;
typedef int16_t __s16;
typedef uint16_t __u16;
typedef int32_t __s32;
typedef uint32_t __u32;
typedef int64_t __s64;
typedef uint64_t __u64;


typedef unsigned long ulong;
typedef unsigned int  uint;
typedef unsigned char uchar;

/*
 * These are used to make use of C type-checking..
 */
typedef unsigned long	pteval_t;
typedef unsigned long	pmdval_t;
typedef unsigned long	pudval_t;
typedef unsigned long	p4dval_t;
typedef unsigned long	pgdval_t;
typedef unsigned long	pgprotval_t;

#define pgoff_t unsigned long

typedef size_t pml4e_t;
typedef size_t pdpe_t;
typedef size_t pte_t;
typedef size_t pde_t;

typedef size_t physaddr_t;

typedef long            phy_base_t;      /**< Nbit CPU related date type */
typedef unsigned long   phy_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef size_t err_t;
typedef size_t      phy_tick_t;      /**< Type for tick count */

typedef struct {
	int counter;
} atomic_t;

typedef struct {
	long counter;
} atomic_long_t;

typedef paddr_t phys_addr_t;

typedef phys_addr_t resource_size_t;

#ifndef offsetof
 #define offsetOf(type,member)  ((pointer)&((type *)0)->member)
#endif

#define containerOf(ptr,type,member) ({ \
   const typeof(((type *)0)->member) *___ = (ptr);\
   (type *)((char *)___ - offsetof(type,member));\
})


#ifdef CONFIG_X86_32
# define BITS_PER_LONG 32
#else
# define BITS_PER_LONG 64
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

 #define offset_in_page(p) (((unsigned long)p) % PAGE_SIZE)

 #define PHYS_ADDR_MAX	(~(phys_addr_t)0)

 #ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif

#define __bitwise __bitwise__

typedef unsigned int __bitwise gfp_t;
typedef unsigned int __bitwise slab_flags_t;
typedef unsigned int __bitwise fmode_t;

 typedef __bitwise unsigned int vm_fault_t;

#ifdef CONFIG_X86_64
#define memptr long
#else
#define memptr unsigned
#define ULONG_MAX  0xffffffff
#endif

// Limits
#ifndef NAME_MAX
 #define NAME_MAX   255
#endif
#ifndef PATH_MAX
 #define PATH_MAX   1024
#endif

#endif
