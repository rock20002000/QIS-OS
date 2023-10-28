#include <platform/include/platform.h>

int main(int argc, char** argv)
{
    struct timespec ts;

    clock_gettime(0, &ts);

    xprintf("time: %d.%d\n", ts.tv_sec, ts.tv_nsec);
    return EOK;
}
