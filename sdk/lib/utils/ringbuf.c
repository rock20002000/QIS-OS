#include <utils/include/ringbuf.h>

error_t ringbuf_init(ringbuf_t* rb, void* ptr, size_t size)
{
    if (!size)
        return ERR_INVALID_ARG;
    if (!ptr)
        ptr = malloc(size);
    if (!ptr)
        return ERR_NO_MEMORY;
    rb->data = (uint8_t*)ptr;
    rb->size = size;
    ringbuf_reset(rb);
    return EOK;
}

size_t ringbuf_write(ringbuf_t* rb, const void* ptr, size_t len)
{
    if (!rb || !ptr || !len)
        return 0;

    const uint8_t* src = (const uint8_t*)ptr;
    size_t size = (rb->roff + rb->size - rb->woff - 1) % rb->size;

    if (!size)
        return 0;
    if (size < len)
        len = size;
    else
        size = len;

    if (rb->woff + len >= rb->size)
    {
        size_t bytes = len + rb->woff - rb->size;

        memcpy(rb->data + rb->woff, src, bytes);
        len -= bytes;
        rb->woff = 0;
        src += bytes;
    }
    if (len)
    {
        memcpy(rb->data + rb->woff, src, len);
        rb->woff += len;
    }

    return len;
}

size_t ringbuf_read(ringbuf_t* rb, void* ptr, size_t len)
{
    if (!rb || !ptr || !len)
        return 0;

    uint8_t* dst = (uint8_t*)ptr;
    size_t size = (rb->woff + rb->size - rb->roff) % rb->size;

    if (!size)
        return 0;
    if (size < len)
        len = size;
    else
        size = len;

    if (rb->roff + len >= rb->size)
    {
        size_t bytes = len + rb->roff - rb->size;

        memcpy(dst, rb->data + rb->roff, bytes);
        len -= bytes;
        rb->roff = 0;
        dst += bytes;
    }
    if (len)
    {
        memcpy(dst, rb->data + rb->roff, len);
        rb->roff += len;
    }

    return size;
}
