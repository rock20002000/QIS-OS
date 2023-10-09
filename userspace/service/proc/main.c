#include <platform/include/platform.h>
#include "load_kef.h"

void * real_idle_thread(void *pParam) {
	for (;;) {
		;
	}
	return 0;
}


#include "debug_dev.h"
#ifdef WIKA_BOARD
uint32_t g_wdVAddr = 0x0;
#endif
void * idle_thread(void *pParam) {
	int	iCounter = 0;
	for (;;) {
#ifdef WIKA_BOARD

		//uint32_t gpio_base = WATCHDOG_DEV_BASE;	//0x0209C000;
		if(g_wdVAddr==0x0){
			g_wdVAddr = mmap_device_io(0x1000, WATCHDOG_DEV_BASE);	//	DEBUG_DEV_BASE);
		}
		if(g_wdVAddr!=0x0){
			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) | (0x1 << 29));	//	MX6X_GPIO_GDIR 0x04
			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) & (~(0x1 << 29)));

			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) | (0x1 << 12));	//	MX6X_GPIO_GDIR 0x04
			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) & (~(0x1 << 12)));

			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) | (0x1 << 4));	//	MX6X_GPIO_GDIR 0x04
			out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) & (~(0x1 << 4)));

//			if((iCounter%600)==0){
//				iCounter = 0;
//				xprintf("Proc feed watch dog flip\n");
//				out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) | (0x1 << 29));	//	MX6X_GPIO_GDIR 0x04
//				//out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) & (~(0x1 << 29)));
//			}
//			iCounter++;
//			if((iCounter%300)==0){
//				//iCounter = 0;
//				xprintf("Proc feed watch dog flop\n");
//				//out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) | (0x1 << 29));	//	MX6X_GPIO_GDIR 0x04
//				out32(g_wdVAddr + 0x04, in32(g_wdVAddr + 0x04) & (~(0x1 << 29)));
//			}
		}

#endif
		syscall(SYSCALL_SCHEDULE_NEXT, 0, 0, 0, 0, 0);
	}
	return 0;
}

int main(int argc, char* argv[])
{
    int iChid 	= -1;
    int iRcvid 	= -1;
    int iRslt	= -1;

    char szTmp[1024] 	= {0};
    char szBuf[1024] 	= {0};
    char szCmd[1024] 	= {0};
    char* pszTmp		= NULL;
    int	iPosTmp			= 0;
    int iProcID			= 0;

	// xprintf("Proc argc=%d, argv=%p, env=%p\n", argc, argv, &g_procEnv);
    //xprintf("As you can see, I can enter ring0 more than once! \n");
	load_kef_init(argc-1, argv+1);

	//pthread_create(0, 0, real_idle_thread, NULL);
	//pthread_create(0, 0, real_idle_thread, NULL);
	//pthread_create(0, 0, idle_thread, NULL);
	pthread_create(0, 0, idle_thread, NULL);

    //load_app(0);	//	basic test
	//load_app(1);	//	malloc test

	//load_app(2);	//	kbd driver
	//load_app(3);	//	kbd app

	//load_app(7);	//	slicing
	//load_app(35);	//	create_process

	load_app(4);	//	serial driver
	load_app(5);	// 	serial app

	//load_app(14);
	//load_app(15);


	//load_app(6);	//	speed
	//scheduleToRun(1);
	//load_app(7);	//	slicing
	//load_app(8);	//	pulse
	//load_app(9);	//	mutex alloc
	//load_app(10);	//	mutex
	//load_app(11);		//	devnow
	//load_app(12);		//	cat
	//load_app(13);		//	ls
	//load_app(14);		//	msg_server
	//load_app(15);		//	msg_client
	//load_app(16);		//	msgdeliver_server
	//load_app(17);		//	msgdeliver_client
	
	// xprintf("Proc while(1)\n");
	// while(1)
	// {
	// 	;
	// }	
	// return 0;


	if ((iChid = ChannelCreateWithPath(0, "procmgr")) == -1) {	//	if ((chid = ChannelCreate(0)) == -1) {
            perror("ChannelCreate");
            return -1;
    }

    sprintf(szTmp, "Server is ready, pid = %d, chid = %x\n", getpid(), iChid);
    screen_printf(szTmp);

	xprintf("QIS OS V0.11 \n");
	xprintf("\n");

	scheduleToRun(0);
	scheduleToRun(0);
	//scheduleToRun(0);


    for (;;) {
    		memset(szBuf, 0, sizeof(szBuf));
    		memset(szCmd, 0, sizeof(szCmd));
    		iPosTmp = 0;
            if ((iRcvid = MsgReceive(iChid, szBuf, sizeof(szBuf), NULL)) == -1) {
                    perror("MsgReceive");
                    return -1;
            }
            //sprintf(szTmp, "Server:receive msg with iRcvid:%d.\n", iRcvid);
            //screen_printf(szTmp);

            //	memcpy(&iProcID, buf, sizeof(int));
            //	iRslt = load_app(iProcID);	//	(buf);

            //sprintf(szTmp, "Server: Received '%d'\n", iProcID);	//	sprintf(szTmp, "Server: Received '%s'\n", buf);
            //screen_printf(szTmp);

            pszTmp = szBuf;
            while(*pszTmp!='\0'){
            	if((*pszTmp!=' ')&&(*pszTmp!='\t')){
            		szCmd[iPosTmp++] = (*pszTmp++);
            	}else{
            		szCmd[iPosTmp] = '\0';
            		break;
            	}
            }
            szCmd[iPosTmp] = '\0';

            //sprintf(szTmp, "szCmd:%s szBuf:%s iPosTmp:%d strlen(szBuf):%d szBuf[iPosTmp]:%s\n", szCmd, szBuf, iPosTmp, strlen(szBuf), (char*)(szBuf+iPosTmp));
            //screen_printf(szTmp);

            iRslt = load_kef_by_name(szCmd, 0, 1, iPosTmp>=strlen(szBuf)?NULL:(char*)(szBuf+iPosTmp));	//	load_app(szBuf);
            //scheduleToRun(0);	//	The exit threads currently be set to the wait list, so the delay run load can't be used after system runs

            MsgReply(iRcvid, 0, &iRslt, sizeof(int));
            //scheduleToRun(0);

            // Based on what we receive, return some message
            //if (strcmp(buf, "Hello") == 0) {
            //        MsgReply(rcvid, 0, "World", strlen("World") + 1);
            //} else if (strcmp(buf, "Ni Hao") == 0) {
            //        MsgReply(rcvid, 0, "Zhong Guo", strlen("Zhong Guo") + 1);
            //} else {
            //        MsgError(rcvid, EINVAL);
            //}
    }

    ChannelDestroy(iChid);


}
