/*
 * argc_argv.c
 *
 *  Created by:
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>



int main(int argc, char* argv[])
{
    int i = 0;
    char szTmp[1024] 	= {0};
    printf_s("Test argc:%d argv for command line:\n", argc);
    for(i=0; i<argc; i++){
    	sprintf(szTmp, "%s\n", argv[i]);
    	printf_s(szTmp);
    }
	//xprintf("Proc argc=%d, argv=%p, env=%p\n", argc, argv, &g_procEnv);

    return 0;
}

