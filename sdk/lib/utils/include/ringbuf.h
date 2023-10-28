#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <types.h>

#ifdef KERNEL
 #define malloc  kmallocz
 #define free   kfree
#endif

typedef struct
{
    uint8_t*    data;
    size_t      size;
    size_t      roff;
    size_t      woff;
} ringbuf_t;

static inline error_t ringbuf_reset(ringbuf_t* rb)
{
    rb->roff = rb->woff = 0;
}

error_t ringbuf_init(ringbuf_t* rb, void* ptr, size_t size);
size_t ringbuf_write(ringbuf_t* rb, const void* ptr, size_t len);
size_t ringbuf_read(ringbuf_t* rb, void* ptr, size_t len);

#endif
