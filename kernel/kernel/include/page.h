
#ifndef __PAGE_NUM_H__
#define __PAGE_NUM_H__

#include <types.h>

/**
 * The page size is 4K bytes always
 */
#ifndef PAGE_SHIFT
 #define PAGE_SHIFT		(12)
#endif
#ifndef PAGE_SIZE
 #define PAGE_SIZE		(1 << PAGE_SHIFT)
#endif
#define PAGE_MASK		(PAGE_SIZE - 1)

#define PA_UP(a)		ALIGN_UP(a, PAGE_SIZE)		// page align up
#define PA_DOWN(a)		ALIGN_DOWN(a, PAGE_SIZE)	// page align down

#define PFN_UP(x)		(((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)		((x) >> PAGE_SHIFT)

typedef uint32_t		pfn_t;
#define INVALID_PN		((pfn_t)-1L)

#define addr_to_pn(a)	((pfn_t)((addr_t)(a) >> PAGE_SHIFT))
#define pn_to_addr(pn)	((addr_t)(pn) << PAGE_SHIFT)

#endif
