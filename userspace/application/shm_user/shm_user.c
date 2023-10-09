#include <libc/include/ctype.h>
#include <platform/include/platform.h>
#include <utils/include/ringbuf.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    error_t err = EOK;

    const char* words[] =
    {
        "Hello",
        "Word",
        "Say",
        "Goodbye",
        "Exit",
        NULL
    };

    int i = 0, fd = EOK;
    ringbuf_t* rb = NULL;
    ringbuf_t* wb = NULL;
    uint8_t* base = NULL;
    size_t size = 2*PAGE_SIZE;
    
    xprintf("------------------------ proc%d: %s ------------------------\n", g_procEnv.pid, argv[0]);
    msleep(30);
    fd = shm_open("ringbuf", O_RDWR, 0);
    if (IS_ERROR(fd))
    {
        err = fd;
        xprintf("proc%d, shm_open ringbuf failed: %d!\n", g_procEnv.pid, err);
        goto end;
    }
    xprintf("proc%d, shm_open: %d\n", g_procEnv.pid, fd);
    msleep(30);
    base = mmap(NULL, size, PROT_READ_WRITE, MAP_SHARED, fd, 0);
    if (!base)
    {
        xprintf("proc%d, mmap shared memory failed!\n", g_procEnv.pid);
        err = ERR_NO_MEMORY;
        goto end;
    }
    xprintf("mmap share memory to %p + %x\n", base, size);

    wb = (ringbuf_t*)base;
    rb = (ringbuf_t*)(base + size / 2);

    xprintf("proc%d start send, wb:%p, rb:%p\n", g_procEnv.pid, wb, rb);
    while (words[i])
    {
        char buf[256];
        size_t rlen = 0;
        size_t wlen = ringbuf_write(wb, words[i], strlen(words[i]) + 1);

        if (!wlen)
            continue;
        xprintf("proc%d, sent: %s\n", g_procEnv.pid, words[i]);

        msleep(30);
        rlen = ringbuf_read(rb, buf, sizeof(buf));
        if (rlen != wlen)
        {
            xprintf("proc%d, ringbuf_read: %d != %d\n", g_procEnv.pid, rlen, wlen);
            break;
        }
        xprintf("proc%d, recvd: %s\n", g_procEnv.pid, buf);
        i++;
    }
end:
    if (base)
    {
        err = munmap(base, size);
        if (IS_ERROR(err))
            xprintf("proc%d, mmumap failed: %d!\n", g_procEnv.pid, err);
    }
    if (IS_OK(fd))
    {
        err = shm_close(fd);
        if (IS_ERROR(err))
            xprintf("proc%d, shm_close failed: %d!\n", g_procEnv.pid, err);
    }
    xprintf("------------------------ $s test end ------------------------\n", argv[0]);

    return err;
}
