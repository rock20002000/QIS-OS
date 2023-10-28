
#include <platform/include/platform.h>


char szTestBuf[1024] = {0};
char szTestData[] = "TestWriteSerial\r\n";

/*	void screen_printf(char* pszTmp){
	while((*pszTmp)!='\0'){
		scr_write(*pszTmp);	//	x64_screen_printchar(*pszTmp);
		pszTmp++;
	}
	return;
}	*/

//static void*  __attribute__((noinline)) user_int_register(int_handler_q* handler, size_t irqnum,
//        void*  dev_id, void* irqdata, long flag, unsigned long offset)



typedef struct cmd_index{
	char* 	pszCmd;
	int		iCmdIndex;
} cmd_index_t;

cmd_index_t cmds[] = { 	{"speed", 6}, 				{"slicing", 7},				{"pulse", 8}, 				{"mutex_alloc", 9}, 			{"mutex", 10}, 			{"devnow", 11},
		 	 	 	 	{"cat", 12},				// {"ls", 13},
						{"msg_server", 14}, 		{"msg_client", 15}, 		{"msgdeliver_svr", 16}, 	{"msgdeliver_clt", 17},
						{"msgAttach_svr", 18},		{"msgAttach_clt", 19}, 		{"pulseAttach", 20}, 		{"intrAttachEvt", 21}, 			{"cache", 22},
						{"calls", 23},				{"channel", 24}, 			{"devnull_ctrl", 25}, 		{"devnull", 26}, 				{"devnullr", 27},
						{"malloc", 28}, 			{"memperf", 29},			{"mmap", 30}, 				{"msg", 31}, 					{"opens", 32},
						{"posix_file", 33},			{"argc_argv", 34},			{"create_process", 35},		{"resmgr_process", 36},			{"iotest", 37} };

int			iCmdCount = 31;

int checkCmdLine(char* pszCmd){
	int iChkRslt = 0;

	if(strcmp(pszCmd, "ls")==0){
		iChkRslt = 1;
		for(int i=0; i<iCmdCount; i++){
			screen_printf(cmds[i].pszCmd);
			screen_printf("     ");
		}
		//	screen_printf("bin\n");
		//	screen_printf("dev\n");

		//screen_printf(SGR_DEBUG "bin\n" );
		//screen_printf(SGR_DEBUG "dev\n" );

		//	screen_printf("Number\n");
		//	screen_printf("Char\n");
		//	screen_printf("Intr\n");
		//	screen_printf("Thread\n");
		//	screen_printf("Serial\n");
	}

	if(strcmp(pszCmd, "hello")==0){
		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		//scheduleToRun(0);	//"Number");
	}
	if(strcmp(pszCmd, "malloc")==0){
		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		//scheduleToRun(1);	//"Hello");
	}
	if(strcmp(pszCmd, "serial")==0){
		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);		//"Intr");
		//scheduleToRun(4);
		//scheduleToRun(5);
	}
	//if(strcmp(pszCmd, "serapp")==0){
	//	screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
	//	scheduleToRun(6);	//"Thread");
	//}
//#if 0	// instead of exec, we should send load_app message to proc0
	if(strcmp(pszCmd, "speed")==0){
		iChkRslt = 1;
		shell_exec(pszCmd);	//	shell_exec(6);
	}

	if(strcmp(pszCmd, "pulse")==0){
		iChkRslt = 1;
		shell_exec(pszCmd);	//	shell_exec(8);
	}

	if(strcmp(pszCmd, "mutex_alloc")==0){
		iChkRslt = 1;
		shell_exec(pszCmd);	//	shell_exec(9);
	}

	if(strcmp(pszCmd, "mutex")==0){
		iChkRslt = 1;
		shell_exec(pszCmd);	//	shell_exec(10);
	}
//#endif
	if(strcmp(pszCmd, "devnow")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(11);	//"Serial");
	}

	if(strcmp(pszCmd, "cat")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(12);	//"Serial");
	}

	//	if(strcmp(pszCmd, "ls")==0){
	//		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
	//		exec(14);	//"Serial");
	//	}

	if(strcmp(pszCmd, "msg_server")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(14);	//"Serial");
	}

	if(strcmp(pszCmd, "msg_client")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(15);	//"Serial");
	}

	if(strcmp(pszCmd, "msgdeliver_server")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(16);	//"Serial");
	}

	if(strcmp(pszCmd, "msgdeliver_client")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(17);	//"Serial");
	}

	if(strcmp(pszCmd, "msgAttach_server")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(18);	//"Serial");
	}

	if(strcmp(pszCmd, "msgAttach_client")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(19);	//"Serial");
	}

	if(strcmp(pszCmd, "pulseAttach")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(20);	//"Serial");
	}

	if(strcmp(pszCmd, "interruptAttachEvent")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(21);	//"Serial");
	}

	if(strcmp(pszCmd, "cache")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(22);	//"Serial");
	}

	if(strcmp(pszCmd, "calls")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(23);	//"Serial");
	}

	if(strcmp(pszCmd, "channel")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(24);	//"Serial");
	}

	if(strcmp(pszCmd, "devnull_control")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(25);	//"Serial");
	}

	if(strcmp(pszCmd, "devnull")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(26);	//"Serial");
	}

	if(strcmp(pszCmd, "devnullr")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(27);	//"Serial");
	}

	if(strcmp(pszCmd, "malloc")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(28);	//"Serial");
	}

	if(strcmp(pszCmd, "memperf")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(29);	//"Serial");
	}

	if(strcmp(pszCmd, "mmap")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(30);	//"Serial");
	}

	if(strcmp(pszCmd, "msg")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(31);	//"Serial");
	}

	if(strcmp(pszCmd, "opens")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(32);	//"Serial");
	}

	if(strcmp(pszCmd, "posix_file")==0){
		iChkRslt = 1;		//screen_printf(pszCmd);	//	printf("call thread: %s\n", pszCmd);
		shell_exec(pszCmd);	//	shell_exec(33);	//"Serial");
	}

	if(iChkRslt==0) {
		if(strlen(pszCmd)>0){
			iChkRslt = shell_exec(pszCmd);
			if(iChkRslt==EOK) iChkRslt = 1;
		}
	}

	if( iChkRslt != 1 ){
		if (pszCmd[0] && pszCmd[0] > ' ')
			screen_printf("Unknown command: ");
		screen_printf(pszCmd);
		screen_printf("\n");	//	printf("%s", pszCmd);
	}

	return iChkRslt;
}


void main(void)
{
    xprintf("Test application with device communication:\n");
    //	int			fd	= open("/dev/ser1", "rw");

    int	iReadLen = 0;
    char	szTmp[1024]		= {0};
    char	szTmpChar		= '\n';
    int		i =0; int iSeq 	= 0;
    int		iChkCmdRslt 	= 0;

	xprintf("Before call driver read/write functions:\n");
	//pthread_join();

    int			fd	= open_org("/dev/ser1", "rw");
	while(fd<0){
		yield();//sleep(10);
		fd	= open_org("/dev/ser1", "rw");
	}
    while(1)
    {

    	//for(int i=0; i<7; i++){
        //write_org(fd, szTestData, 18);

        //	sleep(30);

        iReadLen = read_org(fd, szTmp, 1024);
        if(iReadLen>0){
            //screen_printf(szTmp);	//szTestBuf);

			for(i=0; i<iReadLen; i++){
				if(szTmp[i]==127)szTmp[i]=='\b';
				scr_write(szTmp[i]);
				if(szTmp[i]=='\r'){	//	if((szTmp[i]=='\r')||(szTmp[i]=='\n')){
					//scr_write('\n');
					iChkCmdRslt = checkCmdLine(szTestBuf);
					memset(szTestBuf, 0, sizeof(szTestBuf));
					iSeq = 0;
					if(iChkCmdRslt==0){
						screen_printf("root:QIS 0.11$");
					}
				}else if((szTmp[i]=='\b')||(szTmp[i]==127)){
					iSeq--;
					szTestBuf[iSeq] = '\0';
				}
				else{
					szTestBuf[iSeq++] = szTmp[i];
				}
			}
            //	screen_printf("\nroot:QIS 0.11$");
        }

        //for(int i=0; i<10; i++){
        //	xprintf("SerialPort read %d: %s \n", iReadLen, szTestBuf);
        //}


    	//asm volatile("outb %%al, %%dx;"
        //     :
        //     :"a"('\n'), "Nd"(COM1_PORT));
    }

	close_org(fd);

    return;
}
