
#include <platform/include/platform.h>
#include <vdso.h>

char test_xx[256];
char zzz[] = "szxssssssss";


//inline
//unsigned char inportb(unsigned int port)
//{
//   unsigned char ret;
//   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
//   return ret;
//}



int 	serialOpen	(){		//	char *pathname, char *mode){	//	int 	serialOpen	(const char *pathname, const char *mode){
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
             :"a"('\n'), "Nd"(COM1_PORT));	*/
	//	printf("There is some application open %s\n", pathname);
	return 1;
}

size_t serialRead	(){		//	int fd, void *buff, size_t nbytes){
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
             :"a"('\n'), "Nd"(COM1_PORT));	*/
	//printf("There is some application read %d\n", fd);
	return 1;
}

size_t serialWrite	(){		//	int fd, void *buff, size_t nbytes){	//	int 	serialWrite	(int fd, const void *buff, size_t nbytes){
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
             :"a"('S'), "Nd"(COM1_PORT));

    asm volatile("outb %%al, %%dx;"
             :
             :"a"('\n'), "Nd"(COM1_PORT));	*/
	//printf("There is some application write %d\n", fd);
	return 1;
}

int 	serialClose	(){		//	int fd){
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

#ifdef ARM
#include "debug_dev.h"

//  UART registers, offset from base address
#define	MX1_UART_RXDATA			0x00		//  Receiver Register
#define	MX1_UART_TXDATA			0x40		//  Transmitter Register
#define	MX1_UART_CR1			0x80		//  Control Register 1
#define	MX1_UART_CR2			0x84		//  Control Register 2
#define	MX1_UART_CR3			0x88		//  Control Register 3
#define	MX1_UART_CR4			0x8C		//  Control Register 4
#define	MX1_UART_FCR			0x90		//  FIFO Control Register
#define	MX1_UART_SR1			0x94		//  Status Register 1
#define	MX1_UART_SR2			0x98		//  Status Register 2
#define	MX1_UART_ESC			0x9C		//  Escape Character Register
#define	MX1_UART_TIM			0xA0		//  Escape Timer Register
#define	MX1_UART_BIR			0xA4		//  BRM Incremental Register
#define	MX1_UART_BMR			0xA8		//  BRM Modulator Register
#define	MX1_UART_BRC			0xAC		//  Baud Rate Count Register
#define	MX1_UART_BIPR1			0xB0		//  BRM Incremental Preset Register 1
#define	MX1_UART_BIPR2			0xB4		//  BRM Incremental Preset Register 2
#define	MX1_UART_BIPR3			0xB8		//  BRM Incremental Preset Register 3
#define	MX1_UART_BIPR4			0xBC		//  BRM Incremental Preset Register 4
#define	MX1_UART_BMPR1			0xC0		//  BRM Modulator Preset Register
#define	MX1_UART_BMPR2			0xC4		//  BRM Modulator Preset Register
#define	MX1_UART_BMPR3			0xC8		//  BRM Modulator Preset Register
#define	MX1_UART_BMPR4			0xCC		//  BRM Modulator Preset Register
#define	MX1_UART_TS				0xD0		//  Test Register


#define	MX1_USR2_ADET			(1<<15)		//  Automatic Baud Rate Detect Complete
#define	MX1_USR2_TXFE			(1<<14)		//  Transmit Buffer FIFO Empty
#define	MX1_USR2_DTRF			(1<<13)		//  DTR Edge Triggered Interrupt Flag
#define	MX1_USR2_IDLE			(1<<12)		//  IDLE Condition
#define	MX1_USR2_IRINT			(1<<8)		//  Serial Infrared Interrupt Flag
#define	MX1_USR2_WAKE			(1<<7)		//  WAKE
#define	MX1_USR2_DCDDELT		(1<<6)		//  Data Carrier Detect delta
#define	MX1_USR2_DCDIN			(1<<5)		//  Data Carrier Detect input
#define	MX1_USR2_RTSF			(1<<4)		//  RTS Edge Triggered Interrupt Flag
#define	MX1_USR2_TXDC			(1<<3)		//  Transmitter Complete
#define	MX1_USR2_BRCD			(1<<2)		//  BREAK Condition Detected
#define	MX1_USR2_ORE			(1<<1)		//  Overrun Error
#define	MX1_USR2_RDR			(1<<0)		//  Receive Data Ready

#define FIFO_SIZE 32

#define	MX1_URXD_CHARRDY		(1<<15)		//  Character Ready
#define	MX1_URXD_ERR			(1<<14)		//  Error Detect
#define	MX1_URXD_OVERRUN		(1<<13)		//  Receiver Overrun
#define	MX1_URXD_FRMERR			(1<<12)		//  Frame Error
#define	MX1_URXD_BRK			(1<<11)		//  BREAK detect
#define	MX1_URXD_PRERR			(1<<10)		//  Parity Error

#define    MX1_RXERR    (MX1_URXD_ERR | MX1_URXD_OVERRUN | MX1_URXD_FRMERR | MX1_URXD_BRK | MX1_URXD_PRERR)

enum {
	TTI_BREAK    = 0x100,
	TTI_OVERRUN  = 0x200,
	TTI_FRAME    = 0x300,
	TTI_PARITY   = 0x400,
	TTI_CARRIER  = 0x500,
	TTI_HANGUP   = 0x600,
	TTI_OHW_STOP = 0x700,
	TTI_OHW_CONT = 0x800,
	TTI_OSW_STOP = 0x900,
	TTI_OSW_CONT = 0xa00,
	TTI_QUIT     = 0xb00,
	TTI_SUSP     = 0xc00,
	TTI_INTR     = 0xd00
	};

enum {
	TTO_DATA,
	TTO_STTY,
	TTO_CTRL,
	TTO_EVENT,
	TTO_LINESTATUS
	};

enum {
	TTC_INIT_PROC,
	TTC_INIT_CC,
	TTC_INIT_START,
	TTC_INIT_RAW,
	TTC_INIT_EDIT,
	TTC_INIT_ATTACH,
	TTC_SET_OPTION,
	TTC_TIMER_QUEUE,
	TTC_INIT_PTY,
	TTC_INIT_TTYNAME,
	TTC_TTY_DETACH,
	TTC_INIT_POWER
	};

#define	MX1_USR1_PARITYERR		(1<<15)		//  Parity Error Interrupt Flag
#define	MX1_USR1_RTSS			(1<<14)		//  RTS Pin Status
#define	MX1_USR1_TRDY			(1<<13)		//  Transmitter Ready Interrupt/DMA Flag
#define	MX1_USR1_RTSD			(1<<12)		//  RTS Delta
#define	MX1_USR1_ESCF			(1<<11)		//  Escape Sequence Interrupt Flag
#define	MX1_USR1_FRAMERR		(1<<10)		//  Frame Error Interrupt Flag
#define	MX1_USR1_RRDY			(1<<9)		//  Receiver Ready Interrupt/DMA Flag
#define MX1_USR1_AGTIM			(1<<8)		//  Ageing Timer Interrupt Flag
#define	MX1_USR1_RXDS			(1<<6)		//  Receiver IDLE Interrupt Flag
#define	MX1_USR1_AIRINT			(1<<5)		//  Asynchronous IR WAKE Interrupt Flag
#define	MX1_USR1_AWAKE			(1<<4)		//  Asynchronous WAKE Interrupt Flag


const int	SERIAL_IRQ	= DEBUG_DEV_IRQ;	//	59;
uint32_t	g_uart2VAddr = 0;
void initSerialPort(){
	g_uart2VAddr = mmap_device_io(0x1000, DEBUG_DEV_BASE);	//	UART2_PORT);
}

char readSerialPort(){
	return in32(g_uart2VAddr + MX1_UART_RXDATA);	//(in32(UART2_PORT)&0xff);	//	return inportb(COM1_PORT);
}

void writeSerialPort(int data)
{
	while (!(in32(g_uart2VAddr + MX1_UART_SR1) & MX1_USR1_TRDY));
	out32(g_uart2VAddr + MX1_UART_TXDATA, (unsigned)data);
}
//void writeSerialPort(char szTmp){
//	return out32(UART2_PORT, szTmp);	//	return inportb(COM1_PORT);
//}

int checkSerialPortStatus(){
	return ((in32(g_uart2VAddr + MX1_UART_SR2) & MX1_USR2_RDR) );//&& (byte_count < FIFO_SIZE));
}

void updateSerialPortStatus(){
	if (in32(g_uart2VAddr + MX1_UART_SR1) & MX1_USR1_AGTIM)
		out32(g_uart2VAddr + MX1_UART_SR1, in32(g_uart2VAddr + MX1_UART_SR1)|MX1_USR1_AGTIM);
}
#else
const int	SERIAL_IRQ	= 4;
#define COM1_PORT 0x3f8

void initSerialPort(){
	return;
}

char readSerialPort(){
	return in8(COM1_PORT);	//	return inportb(COM1_PORT);
}

void writeSerialPort(char szTmp){
	return out8(COM1_PORT, szTmp);	//	return inportb(COM1_PORT);
}

int checkSerialPortStatus(){
	return (in8(0x3f8+5)&0x01);
}

void updateSerialPortStatus(){
	return;
}
#endif

// #define LATENCY_TEST
#ifdef LATENCY_TEST
static systime_t latency;
static int latency_times;
static size_t cycles_per_time, cycles_per_us;
static size_t add_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_sum)
{
	if (!p_systime1)
        return 0;

	size_t cycles = p_systime1->cycles;
	size_t ticks = p_systime1->ticks;
    size_t total_cycles = 0L;

	if (p_systime2)
	{
		cycles += p_systime2->cycles;
		ticks += p_systime2->ticks;
		if (cycles >= cycles_per_time)
		{
			ticks++;
			cycles -= cycles_per_time;
		}
        total_cycles = ticks * cycles_per_time + cycles;
	}

	if (p_sum)
	{
		p_sum->ticks = ticks;
		p_sum->cycles = cycles;
	}

	return total_cycles;
}
static size_t sub_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_diff)
{
	if (!p_systime1)
        return 0;

	size_t cycles = p_systime1->cycles;
	size_t ticks = p_systime1->ticks;
    size_t total_cycles = 0L;

	if (p_systime2)
	{
		cycles -= p_systime2->cycles;
		ticks -= p_systime2->ticks;
		if (cycles >= cycles_per_time)
		{
			ticks--;
			cycles += cycles_per_time;
		}
        total_cycles = ticks * cycles_per_time + cycles;
	}

	if (p_diff)
	{
		p_diff->ticks = ticks;
		p_diff->cycles = cycles;
	}

	return total_cycles;
}

static systime_t last_time;
static systime_t last_intr_time;
static int last_latency_times;
#define CHECK_LINE_END
#ifdef CHECK_LINE_END
 static bool command;
#endif
static void print_latency(void)
{
#ifdef CHECK_LINE_END
    if (!command)
        return;
    command = false;
#endif
    if (last_latency_times == latency_times)
        return;
    last_latency_times = latency_times;
    if (!latency_times)
        return;
    size_t ticks = latency.ticks / latency_times;
    size_t cycles = ((latency.ticks % latency_times) * cycles_per_time + latency.cycles) / latency_times;

    cycles += ticks * cycles_per_time;
    xprintf("\nlatency count: %d, total latency: %dticks+%dcycles, average latency: %uus\n",
        latency_times, latency.ticks, latency.cycles, cycles/cycles_per_us);
    // xprintf("\ncnt: %d, latency: %uus\n", latency_times, cycles/cycles_per_us);
}
#endif
char		szTmp[4096] = {0};
int			iMsgCur	  	= 0;
int int_handler_user(int irqNum,void* dev_id, void* irqdata){
	//	draw(); //	printf("Calling nterrupt handler from user application \n");

	//xprintf("Serial port read. \n");
//#ifdef ARM
//	char 	szRecv[1024] 	= {0};
//	int		iRecvLen		= 0;
//	syscall(SYSCALL_PLATFORM_ARM_SERIAL_RECV, szRecv, 0, 0, 0, 0);
//	iRecvLen = strlen(szRecv);
//	sprintf(szTmp+iMsgCur, "%s", szRecv);
//	iMsgCur += iRecvLen;
//#else
#ifdef LATENCY_TEST
extern int vdsoGetSystime(systime_t* st);
    int err = vdsoGetSystime(&last_time);

    if (IS_OK(err))
    {
        systime_t st;
        const vdso_data_t* p_vdso_data = (const vdso_data_t*)g_procEnv.vdso_data;
        
        last_intr_time = p_vdso_data->interrupt_time;
        sub_systime(&last_time, &p_vdso_data->interrupt_time, &st);
        if ((int)st.ticks >= 0)
        {
            add_systime(&latency, &st, &latency);
            latency_times++;
        }
    }
#endif
	char			szMsgTmp	= '\0';
	//InterruptMask(SERIAL_IRQ, 0);

	while(checkSerialPortStatus())	//while(inportb(0x3f8+5)&0x01)
	{
		szMsgTmp = readSerialPort();
		if(szMsgTmp=='\0') continue;
		szTmp[iMsgCur++] = szMsgTmp;
		if(iMsgCur>=1024)iMsgCur=0;
#ifdef CHECK_LINE_END
        if ('\n' == szMsgTmp || '\r' == szMsgTmp)
            command = true;
#endif
		//msgData.szMsg[iCur++] = szMsgTmp;	//	inportb(0x3f8);
		//if(iCur>=msgData.iMsgLen)iCur=0;
	}
	//	msgData.iMsgLen = iCur;

	//xprintf("\n\n\n [Serial port read: %s] \n\n\n", szTmp);
	//xprintf("\n\n\n [Serial port read: %s] \n\n\n", szTmp);
	//xprintf("\n\n\n [Serial port read: %s] \n\n\n", szTmp);

	InterruptUnmask(SERIAL_IRQ, 0);

	updateSerialPortStatus();
//#endif

	return 0;
}

void main(void)
{
    int iStatus 	= 0;
    int devToken 	= 's'<<8 | 'r' ;

    MSG_DATA 	msgData;
    MSG_CONTEXT msgContext;
    int			i 		= 0;
    int			iMsgLen = 0;
    int			iRslt	= 0;

    char		szMsgTmp = '\0';

    DEVICE_FUNCTIONS devFunctions;

    int		iDevID	= 52;
    char*  wx="param pass to INT";

	//IRQ_HANDLER_PARAM irqHandlerParam;
	//irqHandlerParam.iIrqVec 		= SERIAL_IRQ;
	//irqHandlerParam.threadFunction 	= int_handler_user;

#ifdef ARM
    xprintf("Test serial port driver application with arm platform:\n");
#else
    xprintf("Test serial port driver application with x86_64 platform:\n");
#endif
    devFunctions.devopen 	= serialOpen;
    devFunctions.devread 	= serialRead;
    devFunctions.devwrite 	= serialWrite;
    devFunctions.devclose 	= serialClose;
    //	msgData.pszMsg	= &szTmp;
#ifdef LATENCY_TEST
    latency.ticks = 0L;
    latency.cycles = 0L;
    latency_times = 0;
    last_latency_times = 0;
    cycles_per_time = g_procEnv.cycles_per_sec / g_procEnv.systimer_freq;
    cycles_per_us = g_procEnv.cycles_per_sec / 1000000;
#ifdef CHECK_LINE_END
    command = false;
#endif
#endif
    initSerialPort();

    int fdDriver = devRegister(&iStatus, "/dev/ser1", devToken, &devFunctions);
    xprintf("Test driver registered with index: %d\n", fdDriver);

    AttachInterruptC(SERIAL_IRQ, int_handler_user);	//	&irqHandlerParam);	//	user_int_register(int_handler_user, 4, &iDevID, (void*)wx, 0, 0);


    while(1){
    	iRslt = devMsgRecvC(fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fd, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);
    	if(iRslt<=0)continue;

    	if(msgData.iMsgType==MSG_WRITE){
    		for(i = 0; i<msgData.iMsgLen; i++){
        		szMsgTmp = msgData.szMsg[i];
        		writeSerialPort(szMsgTmp); // out8(COM1_PORT, szMsgTmp);	//	asm volatile("outb %%al, %%dx;"::"a"(szMsgTmp), "Nd"(COM1_PORT));
    		}
    	}

    	if(msgData.iMsgType==MSG_READ){
    		strcpy(msgData.szMsg, szTmp);	//	"ReceivedInformation.");
    		msgData.iMsgLen		= iMsgCur;	//	21;

    		memset(szTmp, 0, 1024);
    		iMsgCur = 0;

    		//	strcpy(msgData.szMsg, "ReceivedInformation.");
    		//	msgData.iMsgLen		= 21;
    		//	for(i = 0; i<6; i++){
    		//		msgData.szMsg[i] = readSerialPort();
    		//	}

    		//	unsigned char 	szTmp[1024] = {0};
    		/*int				iCur	  = 0;
    	    while(inportb(0x3f8+5)&0x01){
    	    	szMsgTmp = readSerialPort();
    	    	if(szMsgTmp==0)continue;
    	    	msgData.szMsg[iCur++] = szMsgTmp;	//	inportb(0x3f8);
    	    	if(iCur>=msgData.iMsgLen)iCur=0;
    	    }
    	    msgData.iMsgLen = iCur;*/

        	//	msgData.iMsgLen		= 6;
    	}

    	msgData.iMsgType 	= MSG_DONE;
    	devMsgReplyC(fdDriver, &msgData, sizeof(MSG_DATA), &msgContext);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext);
    	//printf(" ");
#ifdef LATENCY_TEST
        print_latency();
#endif
    }

    return;

}



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
