#include <libc/include/stdio.h>
#include <platform/include/syscalls.h>

#ifndef VPRINTF_CONTEXT_BUFSIZE
 #define VPRINTF_CONTEXT_BUFSIZE    (256)
#endif

/*
#define PRINT_BUF_SIZE 256

static char printbuf[PRINT_BUF_SIZE];
static int head = 0;
static int tail = 0;
void printf_flush(void) {
    if (tail < head) {
        sys_write(&printbuf[tail], head - tail, NULL);
        tail = head;
    } else if (tail > head) {
        sys_write(&printbuf[tail], PRINT_BUF_SIZE - tail, NULL);
        sys_write(&printbuf[0], head, NULL);
        tail = head;
    }
}

static void s_printchar(char ch) {
    printbuf[head] = ch;
    head = (head + 1) % PRINT_BUF_SIZE;
    if ((head < tail && head + 1 == tail) || ch == '\n') {
        // The buffer is full or character is newline. Flush the buffer.
        printf_flush();
    }
}
*/

__weak int put_char(char c)//, void* fd)
{
    return sys_write(&c, 1, NULL);
}

#if 0
void printchar(__unused struct vprintf_context *ctx, char ch)
{
    put_char(ch);
}
static void xprintf_printchar(__unused struct vprintf_context *ctx, char ch) {
	put_char(ch);//s_printchar(ch);
}

void printchar(__unused struct vprintf_context *ctx, char ch)
{
    put_char(ch);
}
#else
void put_ctx(struct vprintf_context *ctx)
{
    int written = sys_write(ctx->buf, ctx->index, NULL);

    if (written > 0)
    {
        if (written < ctx->index)
        {
            memmove(ctx->buf, ctx->buf + written, ctx->index - written);
            ctx->index -= written;
        }
        else
            ctx->index = 0;
    }
}

static void xprintf_printchar(struct vprintf_context *ctx, char ch)
{
    if (ctx && ctx->buf && ctx->size)
    {
        if (ctx->index == ctx->size || !ch)
            put_ctx(ctx);
        else
        {
            ctx->buf[ctx->index++] = ch;
            if ('\n' == ch)
            {
                if (ctx->index < ctx->size)
                    ctx->buf[ctx->index++] = '\r';
                put_ctx(ctx);
            }
        }
    }
    else if (ch)
        put_char(ch);
}

void printchar(__unused struct vprintf_context *ctx, char ch)
{
    xprintf_printchar(ctx, ch);
}
#endif

/// Prints a message. See vprintf() for detailed formatting specifications.
void xprintf(const char *fmt, ...)
{
    char ctx_buf[VPRINTF_CONTEXT_BUFSIZE] = {0};
    struct vprintf_context ctx = {xprintf_printchar, ctx_buf, VPRINTF_CONTEXT_BUFSIZE, 0, NULL};

    va_list vargs;
    va_start(vargs, fmt);
    vprintf_with_context(&ctx, fmt, vargs);
    va_end(vargs);
}
