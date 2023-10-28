#include <arch_syscall.h>
#include <platform/include/kirk_task.h>
#include <utils/include/print_macros.h>

extern char __heap[];

#define MAP_FLAGS_HEAP  (MAP_LAZY | MAP_FIXED | MAP_PRIVATE)
#ifndef HEAP_SIZE
 #define HEAP_SIZE      (4 * 1024 * 1024)
#endif

//#define MDBG    PINFO
#define MDBG    DBG

typedef struct _mem_meta
{
    union
    {
        struct _mem_meta *next;
        void*   ptr;
    };
    size_t  size;
} mem_meta_t;
#define META_SIZE       sizeof(mem_meta_t)

struct
{
    void*           base;
    size_t          size;
    mem_meta_t*     free;
    size_t          used;
    pthread_mutex_t mutex;
} heap;

void malloc_init(void)
{
    pthread_mutex_init(&heap.mutex, NULL);
    // heap.size = 0;   // bss clear to zero
}

void* heap_base(void)
{
    return heap.base;
}

size_t heap_size(void)
{
    return heap.size;
}

void heap_info(const char* title)
{
    if (title)
        PINFO("\n------------------------ %s ------------------------\n", title);
    else
        PINFO("\n----------------------------------------------------------------\n");
    PINFO("heap: %p + %x, used %u\n", heap.base, heap.size, heap.used);

    for (mem_meta_t* m = heap.free; m; m = m->next)
    {
        PINFO(" free: %p + %x\n", m, m->size);
    }
    PINFO("----------------------------------------------------------------\n");
}

static inline void remove_meta(mem_meta_t** pprev, mem_meta_t* meta)
{
    if (*pprev != meta)
        (*pprev)->next = meta->next;
    else
        *pprev = meta->next;
    meta->next = NULL;
}

static mem_meta_t* merge_meta(mem_meta_t* meta)
{
    mem_meta_t** pprev;

    meta->next = NULL;
merge_again:
    pprev = &heap.free;
    for (mem_meta_t* cur = *pprev; cur; *pprev = cur, cur = cur->next)
    {
        if ((uintptr_t)meta == (uintptr_t)cur + cur->size)
        {
            cur->size += meta->size;
            remove_meta(pprev, cur);
            meta = cur;
            goto merge_again;
        }
        else if ((uintptr_t)meta + meta->size == cur)
        {
            remove_meta(pprev, cur);
            meta->size += cur->size;
            goto merge_again;
        }
    }
    return meta;
}

static void put_meta(mem_meta_t* meta, bool freed)
{
    meta = merge_meta(meta);

    mem_meta_t** pprev = &heap.free;

    for (mem_meta_t* cur = *pprev; cur; *pprev = cur, cur = cur->next)
    {
        if (meta->size >= cur->size)
            break;
    }

    if (freed)
        heap.used--;
    if (*pprev)
    {
        meta->next = (*pprev)->next;
        if (meta->size >= (*pprev)->size)
        {
            (*pprev)->next = meta;
            return;
        }
    }
    *pprev = meta;
}

static mem_meta_t* get_meta(size_t size)
{
    mem_meta_t* ret = NULL;
    mem_meta_t** pprev = &heap.free;

    for (mem_meta_t* meta = *pprev; meta; *pprev = meta, meta = meta->next)
    {
        if (meta->size < size)
            continue;

        ret = meta;
        remove_meta(pprev, meta);
        if (meta->size > size + META_SIZE)
        {
            meta = (mem_meta_t*)((uintptr_t)ret + size);
            meta->size = ret->size - size;
            ret->size = size;
            
            put_meta(meta, false);
        }
        ret->next = NULL;
        break;
    }
    // heap_info("get_meta");
    return ret;
}

void* malloc(size_t size)
{
    void* ptr = NULL;

    if (!size)
        return NULL;

    size = ALIGN_UP(size, sizeof(size_t));
    pthread_mutex_lock(&heap.mutex);
    if (!heap.size)
    {
        MDBG("mmap(%p, %x, %x)", __heap, HEAP_SIZE, MAP_FLAGS_HEAP);

        void *addr = mmap(__heap, HEAP_SIZE, PROT_READ_WRITE, MAP_FLAGS_HEAP, -1, 0);

        if (addr != MAP_FAILED)
        {
            heap.base = addr;
            heap.size = HEAP_SIZE;
            heap.free = (mem_meta_t*)heap.base;
            heap.free->next = NULL;
            heap.free->size = heap.size;
            heap.used = 0;
            MDBG("\n");
        }
        else
            MDBG(" failed!\n");
    }

    while (heap.size)
    {
        MDBG("malloc(%x)\n", size);
        mem_meta_t* meta = get_meta(size + META_SIZE);

        if (meta)
        {
            meta->ptr = meta + 1;
            ptr = meta->ptr;
            heap.used++;
            MDBG("malloc freed: %p+%x\n", heap.free, heap.free->size);
            break;
        }

        size_t append_size = ALIGN_UP(size + META_SIZE, PAGE_SIZE);

        if (append_size < HEAP_SIZE)
            append_size = HEAP_SIZE
        // heap_info("malloc");
        MDBG("Ext mmap(%p, %x, %x)", (char *)heap.base + heap.size, append_size, MAP_FLAGS_HEAP);

        char* base = (char *)heap.base + heap.size;
        void* addr = mmap(base, append_size, PROT_READ_WRITE, MAP_FLAGS_HEAP, -1, 0);

        if (addr != MAP_FAILED)
        {
            if((vaddr_t)base != addr)
            {
                munmap(base, append_size);
                PANIC("mmap retrun addr: %x != %p", addr, base);
                break;
            }
            MDBG("\n");
            heap.size += append_size;

            mem_meta_t* meta = (mem_meta_t*)addr;

            meta->size = append_size;
            put_meta(meta, false);
            heap_info("Ext mmap");
        }
        else
        {
            MDBG(" failed!\n");
            break;
        }
    }
    pthread_mutex_unlock(&heap.mutex);

    return ptr;
}

/**
    @brief alloc memory in heap
    @param size alloc bytes
    @return Pointer of alloc memory
*/
void *alloc(size_t size){
	return malloc(size);
}

void* calloc ( size_t n,  size_t size )
{
    if (!(n*size))
        return NULL;

	void* tmp = malloc(n*size);

    if (tmp)
	    memset(tmp, 0, n*size);
	return tmp;
}

//	2022-10 ChuanLi modified to add realloc() for temperary use
void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return NULL;
    if (!size)
    {
        free(ptr);
        return NULL;
    }

	mem_meta_t* meta = (mem_meta_t*)((uintptr_t)ptr - META_SIZE);
	void *ptrNew = malloc(size);

	memcpy(ptrNew, ptr, meta->size - META_SIZE);
	free(ptr);
	return ptrNew;
}

void free(void* ptr)
{
    if (!heap.size || (uintptr_t)ptr <= META_SIZE)
    {
        PWARN("Free invalid pointer: %p!", ptr);
        return;
    }

    mem_meta_t* meta = (mem_meta_t*)((uintptr_t)ptr - META_SIZE);

    pthread_mutex_lock(&heap.mutex);
    if (ptr == meta->ptr && meta->size)
    {
        put_meta(meta, true);
        if (!heap.used)
        {
            MDBG("munmap(%p, %x)", heap.base, heap.size);
            if (!munmap(heap.base, heap.size))
            {
                heap.base = NULL;
                heap.free = NULL;
                heap.size = 0;
                MDBG("\n");
            }
            else
                MDBG(" failed!\n");
        }
    }
    else
    {
        PWARN("Free invalid pointer: %p!", ptr);
    }
    pthread_mutex_unlock(&heap.mutex);
}
