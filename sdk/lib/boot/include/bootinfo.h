#ifndef _KIRK_SYSPAGE_H_
#define _KIRK_SYSPAGE_H_

#include <types.h>


/*In arch/$(ARCH) directory*/
#include <arch_syspage.h>

#ifndef __PAGESIZE
  #define __PAGESIZE 4096
#endif

#define TRUNC(_x,_a)  	((paddr_t)(_x) &~ (((paddr_t)(_a))-1))
#define ROUND(_x,_a)  	TRUNC(((paddr_t)(_x)) + ((paddr_t)(_a)-1), (paddr_t)(_a))
#define TRUNCPG(_x)		TRUNC((_x), __PAGESIZE)
#define ROUNDPG(_x)		ROUND((_x), __PAGESIZE)
#define KILO(k)			((k) * 1024UL)
#define MEG(m)			((m) * (1024UL * 1024UL))
#define GIG(m)			((m) * (1024UL * 1024UL * 1024UL))
#define PTR_DIFF(a,b)	((uint8_t *)(a) - (uint8_t *)(b))
#define MAX_CPU 		16
#define MAX_NUM_FREEMEM_REG 16
#define MAX_RAM_LIST_SIZE	KILO(3)
#define MAX_AVOID_LIST_SIZE	KILO(1)



#define MEMMAP_USABLE                   1
#define MEMMAP_RESERVED                 2
#define MEMMAP_ACPI_RECLAIMABLE         3
#define MEMMAP_ACPI_NVS                 4
#define MEMMAP_BAD_MEMORY               5
#define MEMMAP_IO_ZONE                  6
#define MEMMAP_BOOTLOADER_RECLAIMABLE   0x1000
#define MEMMAP_KERNEL_AND_MODULES       0x1001


struct cpuinfo_entry {
    uint32_t		cpu;
    uint32_t		speed;
    uint32_t		flags;
    uint32_t        smp_hwcoreid;
    uint32_t		ins_cache;
    uint32_t		data_cache;
};

// #define SYSTIME_GET_NS
struct systime_entry {
    size_t      (*get_cycles)(const struct systime_entry* p_systime);
    size_t      cycles_per_sec;
    size_t      timer_freq;
    size_t		boot_time;
    uint32_t    intr;
    uint16_t    code_size;
    uint16_t    channel;
    size_t      st_size;
    addr_t      st_base;
};

struct ram_entry {
    paddr_t addr;
    size_t  size;
};

struct avoid_entry {
    paddr_t start;
    paddr_t end;
};

typedef struct p_region {
    paddr_t start;
    paddr_t end;
} p_region_t;

typedef struct mem_p_regs {
    uint8_t     count;
    uint8_t     reserved[7];
    p_region_t  list[MAX_NUM_FREEMEM_REG];
} mem_p_regs_t;

typedef struct meminfo_entry {
    mem_p_regs_t 			mem_p_regs;
    paddr_t                 kvaddr_offs;
    paddr_t                 alloc_top_paddr;

    struct avoid_entry		*avoid_list;
    struct ram_entry		*ram_list;
    unsigned				avoid_list_size;
    unsigned				ram_list_size;
} meminfo_entry_t;

struct intrinfo_entry {
    paddr_t     vector_base;
    paddr_t     cpu_intr_base;
    uint32_t	num_intr;
    uint32_t    reserved;
    int    	(*id)(void *data);
    void    (*eoi)(void *data);
    int		(*mask)(void *data);
    int		(*unmask)(void *data);
};
typedef struct bsp_device
{
    char name[CONFIG_MAX_NAME_LENTH];
    uint8_t num_intf;
    uint8_t irq_num;
    uint8_t szReserver[6];
    addr_t io_addr;
    size_t io_size;
    union {
        uint64_t    u64;
        uint32_t    u32[2];
        uint16_t    u16[4];
        uint8_t     u8[8];
    } param;
    void* interface[1];
} bsp_device_t;
#define BSP_MAX_DEVICE  8
#define DEV_INIT_INDEX  0

struct smp_entry {
    void (*send_ipi)(void *data);
    uint32_t pending;
    uint32_t reserved;
    void *start_addr;
};

struct gtimer_entry {
	uint				cycles_per_sec;	/* for ClockCycles */
	uint volatile			nsec_tod_adjust;
	uint volatile			nsec;
	uint volatile			nsec_stable;
	unsigned long				nsec_inc;
	unsigned long				boot_time; /* UTC seconds when machine booted */	
	unsigned long				timer_rate;	/* times 10^timer_scale */
			 long				timer_scale;
	unsigned long				timer_load;
	         long				intr;
	unsigned long				epoch;
	unsigned long				flags;	
	unsigned long				spare0;
    unsigned int				rr_interval_mul;
    unsigned long            tbu;
	unsigned long            tbl;
   
	unsigned long				spare[4];
};

typedef struct syspage {
    uint16_t                    size;
    uint8_t                     num_cpu;
    uint8_t                     num_dev;
    uint32_t                    reserved;
    struct cpuinfo_entry        cpuinfo[MAX_CPU];
    struct meminfo_entry        meminfo;
    bsp_device_t*               devices[BSP_MAX_DEVICE];
    struct intrinfo_entry       intrinfo[4];
    struct systime_entry        systime;
    struct smp_entry            smp;
    struct gtimer_entry         gptimer;
    // union {
    //     struct x86_64_syspage_entry	x86_64;
    // } un;
    paddr64_t                   pgdir[MAX_CPU];
    paddr64_t                   lapic_addr[MAX_CPU];
    arch_boot_info_t*           loadinfo;
    paddr_t                     (*early_alloc_func)(meminfo_entry_t* p_meminfo, size_t size, size_t align);
} syspage_t;

#define DBG_DEV_INDEX                           (0)
#define DBG_PUTC_INDEX                          (1)
#define DBG_DEV_INTF_NUMS                       (DBG_PUTC_INDEX + 1)
#define DEB_DEV_PARAM_RETRY                     param.u16[0]

#define bsp_add_device(p_syspage)  do {                             \
    (p_syspage)->devices[(p_syspage)->num_dev] =                    \
        (bsp_device_t*)((char*)(p_syspage) + (p_syspage)->size);    \
    (p_syspage)->num_dev++;                                         \
    } while(0)

#define bsp_current_device(p_syspage)   ((p_syspage)->devices[(p_syspage)->num_dev-1])

#define bsp_get_device(p_syspage, dev_index)    \
    ((dev_index) < ((p_syspage)->num_dev) ?     \
        (p_syspage)->devices[dev_index] : NULL)

#define bsp_device_init(p_syspage, dev_index)    do {               \
        void* device = bsp_get_device(p_syspage, dev_index);        \
        if (device && device->interface[0]) {                       \
            (void (*)(bsp_device_t*))device->interface[0](device);  \
        }                                                           \
    } while(0)

#define bsp_device_interface(p_syspage, dev_index, intf_index)  \
    (bsp_get_device(p_syspage, dev_index) ?                     \
        bsp_get_device(p_syspage, dev_index)->interface[intf_index] : NULL)

#define bsp_device_size(num_intf)    (sizeof(bsp_device_t) + ((num_intf) - 1) * sizeof(void*))

#endif
