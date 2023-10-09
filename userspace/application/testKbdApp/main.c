#include <platform/include/platform.h>



char* pszDevPath 		= "/dev/kbd";
char* pszDevOpenMode 	= "rw";

#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	25

#define TAB_SIZE		4

#define COLOR			0xff

//	void setScreenPosition(int x, int y){
//	    int pos = y * SCREEN_WIDTH + x;
//	    asm_out8(0x3d4, 0x0f);
//	    asm_out8(0x3d5, pos & 0xff);
//	    asm_out8(0x3d4, 0x0e);
//	    asm_out8(0x3d5, (pos >> 8) & 0xff);
//	}
/*
void out8_user(short port, uint8_t tmp) {
	asm volatile("outb %%al, %%dx;"
			 :
			 :"a"(tmp), "Nd"(port));
}

void x64_screen_printchar(char ch) {
    static int x = 0;
    static int y = 0;
    static bool in_esc = false;
    uint16_t *vram = (uint16_t *) (0xffff800000000000+0xb8000);	//	paddr2ptr(0xb8000);

    // Ignore ANSI escape sequences.
    if (in_esc) {
        in_esc = (ch != 'm');
        return;
    }

    if (ch == '\e') {
        in_esc = true;
        return;
    }

    if (ch == '\n' || x >= SCREEN_WIDTH) {
        x = 0;
        y++;
    }

    if (y >= SCREEN_HEIGHT) {
        // Scroll lines.
        int diff = y - SCREEN_HEIGHT + 1;
        for (int from = diff; from < SCREEN_HEIGHT; from++) {
            memcpy(vram + (from - diff) * SCREEN_WIDTH,
                   vram + from * SCREEN_WIDTH, SCREEN_WIDTH * sizeof(uint16_t));
        }

        // Clear the new lines.
        memset(vram + (SCREEN_HEIGHT - diff) * SCREEN_WIDTH, 0,
               SCREEN_WIDTH * sizeof(uint16_t) * diff);

        y = SCREEN_HEIGHT - 1;
    }

    if (ch == '\t') {
        for (int i = TAB_SIZE - (x % TAB_SIZE); i > 0; i--) {
            x64_screen_printchar(' ');
        }
    } else if (ch != '\n' && ch != '\r') {
        vram[y * SCREEN_WIDTH + x] = (COLOR << 8 | ch);
        x++;
    }

    // Move the cursor.
    int pos = y * SCREEN_WIDTH + x;
    out8_user(0x3d4, 0x0f);
    out8_user(0x3d5, pos & 0xff);
    out8_user(0x3d4, 0x0e);
    out8_user(0x3d5, (pos >> 8) & 0xff);

    return;
}
*/

/*void screen_printf(char* pszTmp){
	while((*pszTmp)!='\0'){
		scr_write(*pszTmp);	//	x64_screen_printchar(*pszTmp);
		pszTmp++;
	}
	return;
}*/

#define SGR_ERR      "\e[1;91m"  // Bold red.
#define SGR_WARN     "\e[0;33m"  // Yellow.
#define SGR_WARN_DBG "\e[1;33m"  // Bold yellow.
#define SGR_INFO     "\e[0;96m"  // Cyan.
#define SGR_DEBUG    "\e[1;32m"  // Bold green.
#define SGR_RESET    "\e[0m"

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
		screen_printf(pszCmd);
		screen_printf("\n");	//	printf("%s", pszCmd);
	}

	return iChkRslt;
}

void main(void)
{
    xprintf("Test application with keyboard operations:\n");
    int		iKeyboardIrq 	= 1;
    char 	szTestBuf[1024]	= {0};
    char	szTmp[20]		= {0};
    char	szTmpChar		= '\n';
    int		i =0; int iSeq 	= 0;
    int		iChkCmdRslt 	= 0;
    screen_printf("root:QIS 0.11$");		//	printf("root:QIS 0.11$");	//
    //while(1){
    //	;
    //}

    int		fd				= open_org(pszDevPath, pszDevOpenMode);	//	"rw");	//	int			fd	= open("/dev/ser1", "rw");

    int	iReadLen = 0;

    while(1)	//	InterruptWait(iKeyboardIrq))
    {
    	//	sleep(1);
    	//printf("Before call keyboard driver read/write functions with file descriptor:%d \n", fd);
    	//pthread_join();

    	//	for(int i=0; i<7; i++){
        //	write(fd, szTestData, 18);

    	//while(iReadLen<=0){
        iReadLen = read_org(fd, szTmp, 1024);
    	//}

        if(iReadLen>0){
        	screen_printf(szTmp);	//printf("%s", szTestBuf);

			for(i=0; i<iReadLen; i++){
				if((szTmp[i]=='\r')||(szTmp[i]=='\n')){
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

        	/*	if((szTmp[0]=='\r')||(szTmp[0]=='\n')){
				checkCmdLine(szTestBuf);
				memset(szTestBuf, 0, sizeof(szTestBuf));
				iSeq = 0;
				screen_printf("root:QIS 0.11$");
			}else{
				for(i=0; i<iReadLen; i++){
					szTestBuf[iSeq++] = szTmp[i];
				}
			}	*/
        }


        //	for(int i=0; i<100; i++){
        //		szTmpChar = szTmp[0];
        //		printf("Keyboard Port with handler %d read %d: %s first with %d \n", fd, iReadLen, szTestBuf, szTmpChar);
        //	}

        //}

    	//asm volatile("outb %%al, %%dx;"
        //     :
        //     :"a"('\n'), "Nd"(COM1_PORT));
    }

	close_org(fd);

    return;
}

/*
void main(void)
{
	//static void*  __attribute__((noinline)) user_int_register(int_handler_q* handler, size_t irqnum,
	//        void*  dev_id, void* irqdata, long flag, unsigned long offset)

    printf("Test interrupt handler \n");
    int		iDevID	= 32;
    char*  wx="param pass to INT";
    user_int_register(int_handler_user, 1, &iDevID, (void*)wx, 0, 0);

    while(1)
    {
        //	for(int i=0x30; i<0x60; i++){
        	//	for(int j=0; j<100; j++)
        	{
        		printf("%s\n", szTestBuf);	//, i);
        	}
        	if(strcmp(szTestBuf, "wait")==0){
    			memset(szTestBuf, 0, 1024);
    			iMsgCur=0;
    			waitIntr(1);		//	yield();
        	}
        	if(strcmp(szTestBuf, "quit")==0){
				return;
			}
        //}
    }

}
*/
