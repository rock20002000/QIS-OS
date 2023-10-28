#include <libc/include/ctype.h>
#include <platform/include/platform.h>
#include <utils/include/ringbuf.h>
#include <fcntl.h>

void main(int argc, char* argv[])
{
    error_t err = EOK;

#define SHM_NAME    "ringbuf"
    xprintf("------------------------ proc%d: %s ------------------------\n", g_procEnv.pid, argv[0]);

    int fd = NOFD;
    size_t len = 0;
    offset_t paddr = NULL_PADDR;
    size_t size = 2*PAGE_SIZE;
    ringbuf_t* rb = NULL;
    ringbuf_t* wb = NULL;
    uint8_t* base = mmap(NULL, size, PROT_READ_WRITE, MAP_SHARED_PHYS_ANON, NOFD, 0);

    if (!base)
    {
        xprintf("proc%d, mmap physical memory failed!\n", g_procEnv.pid);
        err = ERR_NO_MEMORY;
        goto end;
    }

    err = moffset(base, size, &paddr, &len);
    if (EOK != err)
    {
        xprintf("proc%d, moffset get physical memory failed: %d!\n", g_procEnv.pid, err);
        goto end;
    }
    xprintf("moffset get physical memory from: %p = %p + %x\n", base, paddr, len);

    fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0);

    if (IS_ERROR(fd))
    {
        err = fd;
        xprintf("proc%d, shm_open failed: %d!\n", g_procEnv.pid, err);
        goto end;
    }

    err = shm_ctl(fd, SHMCTL_PHYS, paddr, len);
    if (IS_ERROR(fd))
    {
        xprintf("proc%d, shm_ctl failed: %d!\n", g_procEnv.pid, err);
        goto end;
    }
    xprintf("proc%d, shm_ctl: %p + %x\n", g_procEnv.pid, paddr, len);


    rb = (ringbuf_t*)base;
    ringbuf_init(rb, (uint8_t*)(rb + 1), len/2 - sizeof(ringbuf_t));
    wb = (ringbuf_t*)(base + len / 2);
    ringbuf_init(wb, (uint8_t*)(wb + 1), len/2 - sizeof(ringbuf_t));

    xprintf("proc%d start receive, rb:%p, wb:%p\n", g_procEnv.pid, rb, wb);
    while (1)
    {
        char buf[256] = {0};

        size_t rlen = ringbuf_read(rb, buf, sizeof(buf));

        if (!rlen)
            continue;

        // xprintf("proc%d, recved: %s!\n", g_procEnv.pid, buf);
        for (size_t i = 0; i < rlen; i++)
        {
            if (islower(buf[i]))
                buf[i] -= 'a' - 'A';
        }

        size_t wlen = ringbuf_write(wb, buf, rlen);

        if (wlen != rlen)
        {
            xprintf("proc%d, ringbuf_write: %d != %d\n", g_procEnv.pid, wlen, rlen);
            break;
        }
        if (!strcmp("EXIT", buf))
        {
            xprintf("proc%d, ringbuf exit\n", g_procEnv.pid);
            break;
        }
    }
end:
    if (IS_OK(fd))
    {
        err = shm_close(fd);
        if (IS_ERROR(err))
            xprintf("proc%d, shm_close failed: %d!\n", g_procEnv.pid, err);
    }
    if (base)
    {
        err = munmap(base, size);
        if (IS_ERROR(err))
            xprintf("proc%d, mmumap failed: %d!\n", g_procEnv.pid, err);
    }
    shm_unlink(SHM_NAME);
    xprintf("------------------------ $s test end ------------------------\n", argv[0]);

    return err;
}
