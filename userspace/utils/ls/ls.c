/*
 * ls.c
 *
 *  Created by:
 *      Author: Test
 */


#include <platform/include/platform.h>


//char szTestBuf[1024] = {0};
//char szTestData[] = "TestWriteSerial\r\n";
int main(int argc, char** argv)
{
    screen_printf("Test ls operation:\n");

    int		iReadLen = 0;
    char 	szTestBuf[1024] = {0};

    int			fd	= open("/dev/now", "r");

    if( fd != -1 ){

    	iReadLen = read(fd, szTestBuf, 1024);
    	if(iReadLen>0){
    		screen_printf(szTestBuf);
    		screen_printf("\nroot:QIS 0.11$");
    	}

    	close(fd);

    }

	return 1;
}




