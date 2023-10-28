/*
 * cat.c
 *
 *  Created on: 2022-9-27
 *      Author: Test
 */

#include <platform/include/platform.h>


//char szTestBuf[1024] = {0};
//char szTestData[] = "TestWriteSerial\r\n";
int main(int argc, char** argv)
{
    screen_printf("Test cat operation:\n");
    //	int			fd	= open("/dev/ser1", "rw");

    int		iReadLen = 0;
    char 	szTestBuf[1024] = {0};

	//xprintf("Before call driver read/write functions:\n");
	//pthread_join();

    int			fd	= open("/dev/now", "r");
    sprintf(szTestBuf, "Test cat operation: after open with handle:%x \n", fd);
    screen_printf(szTestBuf);

    //screen_printf("Test cat operation: after open\n");
    //while(1)
    //{
    //	//for(int i=0; i<7; i++){
    //   write(fd, szTestData, 18);

    //screen_printf("Before sleep. \n");
    //sleep(3000);
    //screen_printf("After sleep. \n");

    if( fd != -1){
    	screen_printf("Test cat operation: before read\n");
    	memset(szTestBuf, 0, 1024);

    	iReadLen = read(fd, szTestBuf, 1024);
    	//screen_printf("Test cat operation: after read\n");
    	if(iReadLen>0){
    		screen_printf(szTestBuf);
    		screen_printf("\nroot:QIS 0.11$");
    	}

    	//for(int i=0; i<10; i++){
    	//	xprintf("SerialPort read %d: %s \n", iReadLen, szTestBuf);
    	//}

        //}

    	screen_printf("Test cat operation: before close\n");
    	close(fd);
    	screen_printf("Test cat operation: after close\n");
    }

	return 1;
}

