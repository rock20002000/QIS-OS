/*
 * create_process.c
 *
 *  Created by:
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>



int main(int argc, char* argv[])
{
    int iRslt	= -1;

    char szTmp[1024] 	= {0};

    shell_exec("argc_argv testParam1 testParam2 testParam3 testParam4 testParam5");
    shell_exec("pulse");
    shell_exec("mutex");
    shell_exec("speed");
    shell_exec("msg_server");
    shell_exec("msg_client");
    shell_exec("pulseAttach");
    shell_exec("intrAttachEvt");
    shell_exec("devnow");
    shell_exec("cat");
    shell_exec("cache");
    shell_exec("calls");
	//	xprintf("Proc argc=%d, argv=%p, env=%p\n", argc, argv, &g_procEnv);

    return 1;
}
