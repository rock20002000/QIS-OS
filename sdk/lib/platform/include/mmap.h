/** \file mmap.h
  *
  * Copyright 2022, KunyuOS Software Systems. All Rights Reserved.
  *
  * @author   	WangYang
  * @email   	yang.wang@kunyuos.com
  * @version	V1.0
  * @date    	2022-09-20
  * @brief   	mmap/mmunmap define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-09-20|Create File
  * V1.1|WangYang|2022-10-18|Sync map flags with POSIX
  */
#ifndef _MMAP_H__
#define _MMAP_H__

#include <types.h>

/**
 * Memory types: Must choose either STACK, PRIVATE, ELF or SHARED
 */
#define MAP_STACK       0x00000001     /* memory used for stack     */
#define MAP_PRIVATE     0x00000002     /* private in process        */
#define MAP_ELF         0x00000004     /* ELF in library            */
#define MAP_SHARED      0x00000008     /* memory used for shared    */
#define MAP_TYPE        0x0000000f     /* mask for memory type      */

/**
 * Flags to mmap (or-ed in to STACK, PRIVATE, ELF or SHARED)
 */
#define MAP_ANON        0x00000040     /* anonymous memory       */
#define MAP_LAZY        0x00000080     /* allow lazy mapping     */
#define MAP_PHYS        0x00010000     /* map physical memory    */
#define MAP_FIXED       0x00020000     /* user assigns address   */

/**
 * Reserved for system use
 */
#define MAP_SPARE1 		0x10000000	   /* reserved for system use */	
#define MAP_SPARE2 		0x20000000	   /* reserved for system use */	
#define MAP_SPARE3 		0x40000000	   /* reserved for system use */	
#define MAP_SPARE4 		0x80000000	   /* reserved for system use */	
#define MAP_RESERVMASK 	0xf0000000	   /* reserved for system use */	

/**
 * Not support now
 */
#define MAP_NOINIT		0x00100000     /* don't have to initialize the memory */
#define MAP_SYSRAM      0x00200000     /* system ram             */
#define MAP_FILE        0x00400000     /* map to file            */
#define MAP_NOSYNCFILE  0x00800000     /* Memory not synced to file */
#define MAP_BELOW       0x01000000     /* allocate below hint    */
#define MAP_NOX64K      0x02000000     /* don't cross 64k bound  */
#define MAP_BELOW16M    0x04000000     /* allocate below 16M     */

#define MAP_PRIVATEANON         (MAP_PRIVATE | MAP_ANON)
#define MAP_SHARED_PHYS_ANON    (MAP_SHARED | MAP_PHYS | MAP_ANON)

/**
 * Page protection flags which may be or-ed together
 */
#define PROT_NONE       0x00000000
#define PROT_READ       0x00000100     /* pages can be read */
#define PROT_WRITE      0x00000200     /* pages can be written  */
#define PROT_EXEC       0x00000400     /* pages can be executed */
#define PROT_NOCACHE    0x00000800     /* pages are not cached  */
#define PROT_MASK       0x00000f00
#define PROT_READ_WRITE (PROT_READ | PROT_WRITE)

#ifndef NOFD
 #define NOFD           NULL_ID
#endif
#define MAP_FAILED      ((void*)NULL_ADDR)

#ifndef PAGE_SHIFT
 #define PAGE_SHIFT		    (12)
#endif
#ifndef PAGE_SIZE
 #define PAGE_SIZE		    (1 << PAGE_SHIFT)
#endif

/**
 * Not support now
 */
#define SHMCTL_ANON			0x00000001			/* Allocate anon memory for object */
#define SHMCTL_PHYS			0x00000002			/* Providing physical memory for object */
#define SHMCTL_ISADMA		0x00000004			/* Memory should be suitable for ISA DMA (e.g. < 16M, NOX64) */
#define SHMCTL_GLOBAL		0x00000010			/* Hint that object is globally visible */
#define SHMCTL_PRIV			0x00000020			/* Hint that accessing object requires privilege */
#define SHMCTL_LOWERPROT	0x00000040			/* Hint that access to this object can use lower protection in exchange for better performance */
#define SHMCTL_LAZYWRITE	0x00000080			/* Hint that mapping of this object could use lazy-writing mechanisms */
#define SHMCTL_NOX64K		0x00000100			/* Memory should not cross 64K boundry */
#define SHMCTL_HIGHUSAGE	0x00000200			/* Hint that the object is high usage and the system should use limited resources for better performance */
#define SHMCTL_LAZY			0x00000400			/* Delay actually allocating memory until first reference */
#define SHMCTL_HAS_SPECIAL	0x00008000
#define SHMCTL_FLAG_MASK	0x0000ffff

typedef struct
{
    vaddr_t     vaddr;
    size_t      size;
    uint32_t    flags;
    id_t        id;
    union
    {
        size_t  off;
        vaddr_t vmem;
    };
} map_params_t;

typedef struct
{
    union
    {
        paddr_t     paddr;
        vaddr_t     vaddr;
    };
    size_t          size;
    uint32_t        flags;
} shm_block_t;

typedef struct
{
    uint32_t total_pages;
    uint32_t free_pages;
    uint32_t shared_pages;
    uint32_t slob_pages;
} mem_info_t;

/**
    @brief Memory map
    @param addr map address
    @param len map size
    @param prot memory protection attribute
    @param flags memory map attribute
    @param id id handle for map, NULL_ID is not exist
    @param off base offset in map file or virtual address in process
    @return map address, 'NULL' is failed
*/
extern void* mmap(void* addr, size_t len, int prot, int flags, id_t id, offset_t off);

/**
    @brief Memory unmap
    @param addr unmap address
    @param len unmap bytes
*/
extern int munmap(void* addr, size_t len);

/**
    @brief Memory map
    @param io physical io address
    @param len map size
    @return map address, 'NULL' is failed
*/
uintptr_t mmap_device_io(size_t len, paddr_t io);


/**
    @brief Get physical address from virtual address
    @param addr virtual address
    @param len area size
    @param p_off physical address pointer for output
    @param p_contiglen continual area size pointer for output
    @return error code, 'EOK' is successful
*/
int moffset(const void* addr, size_t len, offset_t* p_off, size_t* p_contiglen);
#define memoffset(addr, len, fd, off, contiglen)    moffset(addr, len, off, contiglen)

/**
    @brief Get file descriptor that’s associated with the shared “memory object” specified by name
    @param name The name of the shared memory object
    @param oflag The open flag for the object
    @param mode The permission bits for the object
    @return file descriptor or error code if < 0
*/
int shm_open(const char* name, int oflag, int mode);

/**
    @brief Close shared memory object
    @param fd file descriptor returned by shm_open()
    @return error code, 'EOK' is successful
*/
int shm_close(int fd);

/**
    @brief Modifies the attributes of the shared memory object
    @param fd file descriptor returned by shm_open()
    @param flags The flags of the object
    @param paddr A physical address to assign to the object
    @param size The size of the object
    @return map address, 'NULL' is failed
*/
int shm_ctl(int fd, int flags, paddr_t paddr, size_t size);

/**
    @brief Unlink the shared “memory object” specified by name
    @param name The name of the shared memory object
    @return error code, 'EOK' is successful
*/
int shm_unlink(const char* name);

#endif
