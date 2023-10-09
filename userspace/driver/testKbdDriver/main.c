#include <platform/include/platform.h>

struct modifiers_state {
    bool ctrl_left;
    bool ctrl_right;
    bool alt_left;
    bool alt_right;
    bool shift_left;
    bool shift_right;
    bool super_left;
    bool super_right;
    bool fn;
    bool caps_lock;
};


char 	szTestBuf[20] = {0};
int		iMsgCur = 0;
char 	szTestData[] = "TestKeyboard\r\n";

static struct modifiers_state modifiers;

struct spinlock_user __keyboard_lock;			//	static struct spinlock_user __keyboard_lock;

#define COM1_PORT 0x3f8

//inline
//unsigned char inportb(unsigned int port)
//{
//   unsigned char ret;
//   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
//   return ret;
//}

char readSerialPort(){
	return in8(COM1_PORT);	//	return inportb(COM1_PORT);
}


int 	kbdOpen	(){		//	char *pathname, char *mode){	//	int 	serialOpen	(const char *pathname, const char *mode){
/*    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('D'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('i'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('v'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
    		 :
    		 :"a"('\t'), "Nd"(COM1_PORT));



    asm volatile("outb %%al, %%dx;"
             :
             :"a"('O'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('p'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('n'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT)); */
	//	printf("There is some application open %s\n", pathname);
	return 1;
}

int 	kbdRead	(){//int fd, void *buff, size_t nbytes){
/*    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('D'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('i'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('v'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
    		 :
    		 :"a"('\t'), "Nd"(COM1_PORT));


    asm volatile("outb %%al, %%dx;"
             :
             :"a"('R'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('a'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('d'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT)); */
	//printf("There is some application read %d\n", fd);
	return 1;
}

int 	kbdWrite	(){//int fd, void *buff, size_t nbytes){	//	int 	serialWrite	(int fd, const void *buff, size_t nbytes){
/*    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('D'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('i'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('v'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
    		 :
    		 :"a"('\t'), "Nd"(COM1_PORT));


    asm volatile("outb %%al, %%dx;"
             :
             :"a"('W'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('i'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('t'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('K'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));	*/
	//printf("There is some application write %d\n", fd);
	return 1;
}

int 	kbdClose	(){//int fd){
/*    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('D'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('i'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('v'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('r'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
    		 :
    		 :"a"('\t'), "Nd"(COM1_PORT));



    asm volatile("outb %%al, %%dx;"
             :
             :"a"('C'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('l'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('o'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('s'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('e'), "Nd"(COM1_PORT));
    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));	*/
	//printf("There is some application close %d\n", fd);
	return 1;
}

//typedef struct _dev_func{
//	int 	(*devopen)	(const char *pathname, const char *mode);
//	size_t 	(*devread)	(int fd, void *buff, size_t nbytes);
//	size_t 	(*devwrite)	(int fd, const void *buff, size_t nbytes);
//	int 	(*devclose)	(int fd);
//}DEVICE_FUNCTIONS;

//	int devMsgRecv(int __chid, void *__msg, int __bytes, struct _msg_data *__info){
//		return syscall(SYSCALL_MSG_RECV, __chid, __msg, __bytes, __info, 0);
//	}
//	int devMsgReply(int __chid, void *__msg, int __bytes, MSG_CONTEXT *__info){
//		return syscall(SYSCALL_MSG_REPLY, __chid, __msg, __bytes, __info, 0);
//	}


//static void*  __attribute__((noinline)) user_int_register(int_handler_q* handler, size_t irqnum,
//        void*  dev_id, void* irqdata, long flag, unsigned long offset)


#define KEY_RELEASE 0x80

enum keycode {
    KEY_ENTER = 0x00,
    KEY_BACKSPACE = 0x01,
    KEY_CTRL_LEFT = 0x02,
    KEY_CTRL_RIGHT = 0x03,
    KEY_ALT_LEFT = 0x04,
    KEY_ALT_RIGHT = 0x05,
    KEY_SHIFT_LEFT = 0x06,
    KEY_SHIFT_RIGHT = 0x07,
    KEY_SUPER_LEFT = 0x08,
    KEY_SUPER_RIGHT = 0x09,
    KEY_FN = 0x0a,
    KEY_CAPS_LOCK = 0x0b,
    KEY_ARROW_UP = 0x0c,
    KEY_ARROW_DOWN = 0x0d,
    KEY_ARROW_LEFT = 0x0e,
    KEY_ARROW_RIGHT = 0x0f,
    KEY_ESC = 0x10,
    KEY_F1 = 0x11,
    KEY_F2 = 0x12,
    KEY_F3 = 0x13,
    KEY_F4 = 0x14,
    KEY_F5 = 0x15,
    KEY_F6 = 0x16,
    KEY_F7 = 0x17,
    KEY_F8 = 0x18,
    KEY_F9 = 0x19,
    KEY_F10 = 0x1a,
    KEY_F11 = 0x1b,
    KEY_F12 = 0x1c,
};


uint8_t us_key_map[] = {
    0, KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    KEY_BACKSPACE,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', KEY_ENTER,
    KEY_CTRL_LEFT, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    KEY_SHIFT_LEFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    KEY_SHIFT_RIGHT,
    0, KEY_ALT_LEFT, ' ', KEY_CAPS_LOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    KEY_F11, KEY_F12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t us_shifted_key_map[] = {
    0, KEY_ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    KEY_BACKSPACE,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', KEY_ENTER,
    KEY_CTRL_LEFT, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    KEY_SHIFT_LEFT, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    KEY_SHIFT_RIGHT,
    0, KEY_ALT_LEFT, ' ', KEY_CAPS_LOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    KEY_F11, KEY_F12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t keycode2char(uint8_t ikeycode) {
	uint8_t tmpChar = 0x0;

	if( 	   (ikeycode=='a') || (ikeycode=='b') || (ikeycode=='c') || (ikeycode=='d') || (ikeycode=='e') || (ikeycode=='f') || (ikeycode=='g')
			|| (ikeycode=='h') || (ikeycode=='i') || (ikeycode=='j') || (ikeycode=='k') || (ikeycode=='l') || (ikeycode=='m') || (ikeycode=='n')
			|| (ikeycode=='o') || (ikeycode=='p') || (ikeycode=='q') || (ikeycode=='r') || (ikeycode=='s') || (ikeycode=='t') || (ikeycode=='u')
			|| (ikeycode=='v') || (ikeycode=='w') || (ikeycode=='x') || (ikeycode=='y') || (ikeycode=='z') )
	{
		return ikeycode;
	}else if( 	   (ikeycode=='A') || (ikeycode=='B') || (ikeycode=='C') || (ikeycode=='D') || (ikeycode=='E') || (ikeycode=='F') || (ikeycode=='G')
			|| (ikeycode=='H') || (ikeycode=='I') || (ikeycode=='J') || (ikeycode=='K') || (ikeycode=='L') || (ikeycode=='M') || (ikeycode=='N')
			|| (ikeycode=='O') || (ikeycode=='P') || (ikeycode=='Q') || (ikeycode=='R') || (ikeycode=='S') || (ikeycode=='T') || (ikeycode=='U')
			|| (ikeycode=='V') || (ikeycode=='W') || (ikeycode=='X') || (ikeycode=='Y') || (ikeycode=='Z') )
	{
		return ikeycode;
	}else if( 	   (ikeycode=='0') || (ikeycode=='1') || (ikeycode=='2') || (ikeycode=='3') || (ikeycode=='4') || (ikeycode=='5') || (ikeycode=='6')
			|| (ikeycode=='7') || (ikeycode=='8') || (ikeycode=='9') )
	{
		return ikeycode;
	}

	switch(ikeycode){
	case KEY_ENTER:
		tmpChar = '\n';	//'\r';
		break;
	case KEY_BACKSPACE:
		tmpChar = '\b';
		break;

	default:
		tmpChar = ikeycode;//'\0';
	}

	return tmpChar;	//	0x0;
}

//	inline unsigned char inportb(unsigned int port)
//	{
//	   	unsigned char ret;
//	   	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
//   	return ret;
//	}

uint8_t scan_code2key_code(uint8_t scan_code, bool shift) {
    uint8_t *key_map = shift ? us_shifted_key_map : us_key_map;
    bool released = (scan_code >= 0x80);
    return key_map[scan_code & 0x7f] | (released ? KEY_RELEASE : 0x00);
}

/*
int int_handler_unlock(int irqNum,void* dev_id, void* irqdata){
	spinlock_release_rawuser(&__keyboard_lock);
	return 0;
}
*/

//int int_handler_notify(int irqNum,void* dev_id, void* irqdata){
//	int		iKeyboardIrq	= 1;
//	NotifyInterruptEvent(iKeyboardIrq);
//	return 0;
//}

int int_handler_polling(){	//int irqNum,void* dev_id, void* irqdata){
	//xprintf("Test interrupt keyboard handler\n");
	uint8_t nStatus 	= 0;
	char 	szTmp[2] 	= {0};
	uint8_t iTmp 		= 0;
	uint8_t shift 		= 0;

	while((in8(0x64) & 0x01) != 0)		//	while((inportb(0x64) & 0x01) != 0)
	{
		//nStatus = inportb(0x64);
		//while (( inportb(0x64) & 0x01) != 0);	//nStatus=inportb(0x64);
			szTmp[0] = in8(0x60);		//	inportb(0x60);
			iTmp	= szTmp[0];
			shift = modifiers.shift_left || modifiers.shift_right;

			if(iTmp>128){
				//	iTmp -= 128;

				uint8_t key_code = scan_code2key_code(iTmp, shift);

			    char ascii = '_';
			    switch (key_code & ~KEY_RELEASE) {
			        case KEY_SHIFT_LEFT:
			            modifiers.shift_left = (key_code & KEY_RELEASE) == 0;
			            break;
			        case KEY_SHIFT_RIGHT:
			            modifiers.shift_right = (key_code & KEY_RELEASE) == 0;
			            break;
			        default:
			            ascii = (char) (key_code & ~KEY_RELEASE);
			    }

			    if( ((key_code & ~KEY_RELEASE)==KEY_SHIFT_LEFT)
			    	||((key_code & ~KEY_RELEASE)==KEY_SHIFT_LEFT)){
			    	continue;
			    }
				szTmp[0] = keycode2char(key_code);

				/*if(szTmp[0]!=0x0){
					//	scr_write(szTmp[0]);

					spinlock_acquire_rawuser(&__keyboard_lock);
					szTestBuf[iMsgCur] = szTmp[0];
					if(iMsgCur++>=20)	iMsgCur=0;
					spinlock_release_rawuser(&__keyboard_lock);
					//if(szTmp[0]==' '){
					//	memset(szTestBuf, 0, 20);
					//	iMsgCur=0;
					//}
				}
				xprintf("Test interrupt keyboard release data: [ %d ] %s %d %s nStatus:%d ....\n", iTmp, szTmp, iMsgCur, szTestBuf, nStatus);*/
				//xprintf("Test interrupt keyboard release data: [ %d ] %s %d %s ....\n", iTmp, szTmp, iMsgCur, szTestBuf);
			}else{
				uint8_t key_code = scan_code2key_code(iTmp, shift);

			    char ascii = '_';
			    switch (key_code & ~KEY_RELEASE) {
			        case KEY_SHIFT_LEFT:
			            modifiers.shift_left = (key_code & KEY_RELEASE) == 0;
			            break;
			        case KEY_SHIFT_RIGHT:
			            modifiers.shift_right = (key_code & KEY_RELEASE) == 0;
			            break;
			        default:
			            ascii = (char) (key_code & ~KEY_RELEASE);
			    }
			    if( ((key_code & ~KEY_RELEASE)==KEY_SHIFT_LEFT)
			    	||((key_code & ~KEY_RELEASE)==KEY_SHIFT_LEFT)){
			    	continue;
			    }

				szTmp[0] = keycode2char(key_code);

				if(szTmp[0]!=0x0){
					//	scr_write(szTmp[0]);

					pthread_spin_lock(&__keyboard_lock);
					szTestBuf[iMsgCur] = szTmp[0];
					if(iMsgCur++>=20)	iMsgCur=0;
					pthread_spin_unlock(&__keyboard_lock);
					//if(szTmp[0]==' '){
					//	memset(szTestBuf, 0, 20);
					//	iMsgCur=0;
					//}
				}
				//xprintf("Test interrupt keyboard press data: [ %d ] %s %d %s ....\n", iTmp, szTmp, iMsgCur, szTestBuf);
				//xprintf("Test interrupt keyboard press data: [ %d ] %s %d %s ....\n", iTmp, szTmp, iMsgCur, szTestBuf);

			}
	}


	return 0;
}
/*
int  readKeyboard(char* pszMsgBuf, int iBufLen){
	char szTmp[2] 	= {0};
	uint8_t  iTmp 	= 0;
	int	iMsgLength	= 0;


	//szTmp[0] = inportb(0x60);
    while ((inportb(0x64) & 0x02) != 0);	//	while(szTmp[0]!='\0')	//
    //do
    {
		//while ((inportb(0x64) & 0x02) != 0);
		szTmp[0] = inportb(0x60);
    	iTmp	= szTmp[0];
    	if(iTmp>128){
    		iTmp -= 128;
    		uint8_t key_code = scan_code2key_code(iTmp, 0);
    		szTmp[0] = keycode2char(key_code);

    		if(szTmp[0]!=0x0){
        		pszMsgBuf[iMsgLength] = szTmp[0];
        		if(iMsgLength++>=iBufLen)	iMsgLength=0;
        		//if(szTmp[0]==' '){
        		//	memset(szTestBuf, 0, 20);
        		//	iMsgCur=0;
        		//}
    		}
    		//printf("Test interrupt keyboard release data:[ %d ] %s ....\n", szTmp[0], szTmp);
    	}else{
    		uint8_t key_code = scan_code2key_code(iTmp, 0);
    		szTmp[0] = keycode2char(key_code);
    		//printf("Test interrupt keyboard press data:[ %d ] %s ....\n", szTmp[0], szTmp);
    	}
    	//szTmp[0] = inportb(0x60);
    }//while(szTmp[0]!='\0');

	return iMsgLength;
}
*/

void main(void)
{
    int iStatus 	= 0;
    int devToken 	= 'k'<<8 | 'b' ;

    MSG_DATA 	msgData;
    MSG_CONTEXT msgContext;
    int			i 		= 0;
    int			iMsgLen = 0;
    int			iRslt	= 0;

    char		szMsgTmp = '\0';

    DEVICE_FUNCTIONS devFunctions;

    int		iDevID			= 49;
    int		iKeyboardIrq	= 1;
    char*  	wx				= "param pass to INT";

	//	IRQ_HANDLER_PARAM irqHandlerParam;
	//	irqHandlerParam.iIrqVec 		= iKeyboardIrq;
	//	irqHandlerParam.threadFunction 	= int_handler_polling;

    xprintf("Test keyboard driver operations:\n");

    devFunctions.devopen 	= kbdOpen;
    devFunctions.devread 	= kbdRead;
    devFunctions.devwrite 	= kbdWrite;
    devFunctions.devclose 	= kbdClose;

    //	msgData.pszMsg	= &szTmp;

    int fdDriver = devRegister(&iStatus, "/dev/kbd", devToken, &devFunctions);

    pthread_spin_init(&__keyboard_lock, 0);

    AttachInterruptC(iKeyboardIrq, int_handler_polling);	//	&irqHandlerParam);	//	iKeyboardIrq, int_handler_polling);
    //	(int_handler_unlock, iKeyboardIrq, &iDevID, (void*)wx, 0, 0);	//	user_int_register(int_handler_notify, iKeyboardIrq, &iDevID, (void*)wx, 0, 0);	// the syscall can't be used during interrupt

    xprintf("Test keyboard driver registered with index: %d\n", fdDriver);

    while(1){
    	//printf("Test keyboard driver lock with index: %d\n", fdDriver);


    	//	???
    	//spinlock_acquire_rawuser(&__keyboard_lock);


    	//NotifyInterruptEvent(iKeyboardIrq);

    	//	if(spinlock_tryacquire_rawuser(&__keyboard_lock)==0){
    	//		NotifyInterruptEvent(iKeyboardIrq);
    	//		spinlock_release_rawuser(&__keyboard_lock);
    	//	}
		//if(iMsgCur>0){
		//	//	printf("Before keyboard driver NotifyInterruptEvent\n");
		//	NotifyInterruptEvent(iKeyboardIrq);
		//}

		//printf("Test receive message from keyboard driver with driver index: %d\n", fdDriver);
    	iRslt = devMsgRecvC(fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fd, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);

    	/* szTestBuf[0]='N';
    	szTestBuf[1]='o';
    	szTestBuf[2]='D';
    	szTestBuf[3]='a';
    	szTestBuf[4]='t';
    	szTestBuf[5]='a'; */

    	if(iRslt<=0){
    		continue;
    	}

    	/* if(msgData.iMsgType==MSG_WRITE){
    		for(i = 0; i<msgData.iMsgLen; i++){
        		szMsgTmp = msgData.szMsg[i];
        		asm volatile("outb %%al, %%dx;"
        		             :
        		             :"a"(szMsgTmp), "Nd"(COM1_PORT));
    		}
    	}	*/

    	if(msgData.iMsgType==MSG_READ){
    		//iMsgLen = readKeyboard(msgData.szMsg, 20);	//	msgData.iMsgLen);

    		if(iMsgCur>0){
    			pthread_spin_lock(&__keyboard_lock);
        		strcpy(msgData.szMsg, szTestBuf);	//	"ReceivedInformation.");
        		msgData.szMsg[iMsgCur]	= '\0';
        		msgData.iMsgLen			= iMsgCur;	//	iMsgLen;	//	21;

        		memset(szTestBuf, 0, 20);
        		iMsgCur = 0;
        		pthread_spin_unlock(&__keyboard_lock);
    		}else{
        		msgData.szMsg[0]	= '\0';
        		msgData.iMsgLen		= 0;	//	iMsgLen;	//	21;
    		}
    	}

    	//printf("Test reply message from keyboard driver with driver index: %d\n", fdDriver);
    	msgData.iMsgType 	= MSG_DONE;
    	devMsgReplyC(fdDriver, &msgData, sizeof(MSG_DATA), &msgContext);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext);
    }

    return;

}


/*
int int_handler_polling(int irqNum,void* dev_id, void* irqdata){
	char szTmp[2] 	= {0};
	uint8_t  iTmp 	= 0;

    while ((inportb(0x64) & 0x02) != 0);
	szTmp[0] = inportb(0x60);
	iTmp	= szTmp[0];
	if(iTmp>128){
		iTmp -= 128;
		uint8_t key_code = scan_code2key_code(iTmp, 0);
		szTmp[0] = key_code;

		szTestBuf[iMsgCur] = szTmp[0];
		if(iMsgCur++>=20)	iMsgCur=0;
		if(szTmp[0]==' '){
			memset(szTestBuf, 0, 20);
			iMsgCur=0;
		}

		//	kprintf("Test interrupt keyboard release data:[ %d ] %s ....\n", iTmp, szTmp);
	}else{
		uint8_t key_code = scan_code2key_code(iTmp, 0);
		szTmp[0] = key_code;
		//	kprintf("Test interrupt keyboard press data:[ %d ] %s ....\n", iTmp, szTmp);

	}

	return 0;
}

void main(void)
{
    int iStatus 	= 0;
    int devToken 	= 'k'<<8 | 'b' ;

    MSG_DATA 	msgData;
    MSG_CONTEXT msgContext;
    int			i 		= 0;
    int			iMsgLen = 0;
    int			iRslt	= 0;

    char		szMsgTmp = '\0';

    DEVICE_FUNCTIONS devFunctions;

    int		iDevID			= 49;
    int		iKeyboardIrq	= 1;
    char*  wx="param pass to INT";

    printf("Test keyboard driver operations:\n");

    devFunctions.devopen 	= kbdOpen;
    devFunctions.devread 	= kbdRead;
    devFunctions.devwrite 	= kbdWrite;
    devFunctions.devclose 	= kbdClose;

    //	msgData.pszMsg	= &szTmp;

    int fdDriver = devRegister(&iStatus, "/dev/kbd", devToken, &devFunctions);
    printf("Test keyboard driver registered with index: %d\n", fdDriver);

    user_int_register(int_handler_polling, iKeyboardIrq, &iDevID, (void*)wx, 0, 0);


    while(1){

		if(iMsgCur>0){
			//	printf("Before keyboard driver NotifyInterruptEvent\n");
			NotifyInterruptEvent(iKeyboardIrq);
		}

		//printf("Test receive message from keyboard driver with driver index: %d\n", fdDriver);
    	iRslt = devMsgRecv(0, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fd, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);

    	/ * szTestBuf[0]='N';
    	szTestBuf[1]='o';
    	szTestBuf[2]='D';
    	szTestBuf[3]='a';
    	szTestBuf[4]='t';
    	szTestBuf[5]='a'; * /

    	if(iRslt<=0){
    		continue;
    	}

    	/ *if(msgData.iMsgType==MSG_WRITE){
    		for(i = 0; i<msgData.iMsgLen; i++){
        		szMsgTmp = msgData.szMsg[i];
        		asm volatile("outb %%al, %%dx;"
        		             :
        		             :"a"(szMsgTmp), "Nd"(COM1_PORT));
    		}
    	}	* /

    	if(msgData.iMsgType==MSG_READ){
    		strcpy(msgData.szMsg, szTestBuf);	//	"ReceivedInformation.");
    		msgData.iMsgLen		= iMsgCur;	//	21;

    		memset(szTestBuf, 0, 20);
    		iMsgCur = 0;
    	}

    	//printf("Test reply message from keyboard driver with driver index: %d\n", fdDriver);
    	msgData.iMsgType 	= MSG_DONE;
    	devMsgReply(0, &msgData, sizeof(MSG_DATA), &msgContext);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext);
    }

    return;

}

*/

/*
 *
int spinlock_get(int iTmp){
	return iTmp;
}

int spinlock_set(int iTmp){
	return iTmp;
}


    int 	fd;
    char 	szBuf[1024] = {0};
    int 	iLen	= 0;
	struct _msg_data msgData;


    while(1){
    	while(spinlock_get(iStatus)==0){
        	devThreadWait();
        }
        if(devMsgRecv(&fd, &szBuf, &iLen, &msgData)==-1){
        	iStatus=0;
        	continue;
        }

        if(msgData.iMsgType==MSG_READ){
        	devMsgReply(&fd, &szBuf, &iLen, &msgData);
        }else if(msgData.iMsgType==MSG_WRITE){
        	devMsgReply(&fd, &szBuf, &iLen, &msgData);
        }else{
        	devMsgReply(&fd, &szBuf, &iLen, &msgData);
        }

        spinlock_set(iStatus--);
    }
*/


/*
pthread_t	threadTmp;
int			iDevID	= 32;

//thread_func_user(NULL);
pthread_create(&threadTmp, NULL, thread_func_user, NULL);	//0, &iDevID, NULL, 0, 0);

while(1)
{
	//pthread_join();
    for(int i=0x30; i<g_iSize; i++){
		asm volatile("outb %%al, %%dx;"
             :
             :"a"(g_charToOutput), "Nd"(COM1_PORT));
    	//	for(int j=0; j<100; j++)
    	//{
    	//	printf("*");	//, i);
    	//}
    }
	asm volatile("outb %%al, %%dx;"
         :
         :"a"('\n'), "Nd"(COM1_PORT));
}
*/
