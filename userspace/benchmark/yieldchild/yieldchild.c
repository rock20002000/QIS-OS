#include <platform/include/platform.h>


int main(int argc, char *argv[])
{
	int	    loop;
    int		limit;
    limit = 1000;
    xprintf("-----------------yield child process started-----------------\r\n");
    setprio_critical(0,20);

    for (loop = limit; loop != 0; --loop)
    {
        int res = yield();
        //xprintf("yield child process,current loop number is %d \r\n",loop);
    }	
    xprintf("-----------------yield child process,main is going to exit-----------------\r\n");
		
	return(0);
}