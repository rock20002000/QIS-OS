#include <platform/include/compiler.h>

#include "task.h"
#include "arch_task.h"
#include "kernel.h"
#include "ltimer.h"
#include "aether_resmgr.h"
#include <platform/arch/arm/include/intr/gic.h>
extern syspage_t* g_syspage_ptr;

size_t interrupt_handler_dummy(int irqnum, void* data){
	return 0;
}

/*
#define COM1_PORT 0x3f8

//inline
unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}



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
		tmpChar = '\0';
	}

	return tmpChar;	//	0x0;
}
uint8_t scan_code2key_code(uint8_t scan_code, bool shift) {
    uint8_t *key_map = shift ? us_shifted_key_map : us_key_map;
    bool released = (scan_code >= 0x80);
    return key_map[scan_code & 0x7f] | (released ? KEY_RELEASE : 0x00);
}
static size_t interrupt_keyboard(iframe_t *iframe){
	size_t rsp = (size_t)iframe;

	paddr_t pageTableAddress = arch_pte_backup();

	char szTmp[2] 	= {0};
	uint8_t  iTmp 	= 0;

    while ((inportb(0x64) & 0x02) != 0);
	szTmp[0] = inportb(0x60);
	iTmp	= szTmp[0];
	if(iTmp>128){
		iTmp -= 128;
		uint8_t key_code = scan_code2key_code(iTmp, 0);
		szTmp[0] = keycode2char(key_code);

		if(szTmp[0]!=0x0){
			put_vga(szTmp[0]);	//	scr_write(szTmp[0]);

			//spinlock_acquire_rawuser(&__keyboard_lock);
			//szTestBuf[iMsgCur] = szTmp[0];
			//if(iMsgCur++>=20)	iMsgCur=0;
			//spinlock_release_rawuser(&__keyboard_lock);
			//if(szTmp[0]==' '){
			//	memset(szTestBuf, 0, 20);
			//	iMsgCur=0;
			//}
		}

		printf("Test interrupt keyboard release data: [ %d ] %s ....\n", iTmp, szTmp);
		//xprintf("Test interrupt keyboard release data: [ %d ] %s %d %s ....\n", iTmp, szTmp, iMsgCur, szTestBuf);
	}else{
		uint8_t key_code = scan_code2key_code(iTmp, 0);
		szTmp[0] = keycode2char(key_code);;


		if(szTmp[0]!=0x0){
			//put_vga(szTmp[0]);	//	scr_write(szTmp[0]);

			//spinlock_acquire_rawuser(&__keyboard_lock);
			//szTestBuf[iMsgCur] = szTmp[0];
			//if(iMsgCur++>=20)	iMsgCur=0;
			//spinlock_release_rawuser(&__keyboard_lock);
			//if(szTmp[0]==' '){
			//	memset(szTestBuf, 0, 20);
			//	iMsgCur=0;
			//}
		}


		printf("Test interrupt keyboard press data: [ %d ] %s ....\n", iTmp, szTmp);

		//xprintf("Test interrupt keyboard press data:[ %d ] %s ....\n", iTmp, szTmp);

	}
	arch_pte_restore(pageTableAddress);
	return rsp;
}
*/

void interrupt_forceschedule(iframe_t *iframe){
	int	fScheduleInNeed = 1;
	//	uint64_t rsp = (uint64_t)iframe;
	paddr_t pageTableAddress = arch_pte_backup();

	if(CURRENT_THREAD->schedulePolicy==SCHEDULE_POLICY_FIFO){
		if(CURRENT_THREAD->priority>=THREAD_PRIORITY_LEVEL_NODISTURB){
			fScheduleInNeed = 0;
		}
	}

	if(fScheduleInNeed){	//	if(pPrivilegeThread != NULL){
		THREAD_t* pPrivilegeThread = thread_privilegequeue_pop();
		if(pPrivilegeThread!=NULL){
			start_sched_intr_privilege(pPrivilegeThread, iframe);
		}else{
			arch_pte_restore(pageTableAddress);
		}
	}else{
		arch_pte_restore(pageTableAddress);
	}

	return;// rsp;
}

char		g_szSerialMsgTmp[1024] = {0};
int			g_iSerialMsgCur	  	= 0;
#if 0//def ARCH_ARM
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


static size_t interrupt_serial(iframe_t *iframe){
	size_t rsp = (size_t)iframe;
	paddr_t pageTableAddress = arch_pte_backup();


	/////////////////////////////////////////////////////////////////////////////

	// int            status = 0;
	int            byte_count = 0;
	unsigned       key, rxdata;
	uintptr_t      base = g_syspage_ptr->devices[0]->io_addr;	//	debug_dev.io_addr ;	//dev->base;

	//  limit loop iterations by FIFO size to prevent ISR from running too long
	while ((in32(base + MX1_UART_SR2) & MX1_USR2_RDR) && (byte_count < FIFO_SIZE))
	{
		// Read next character from FIFO

		rxdata = in32(base + MX1_UART_RXDATA);
		key = rxdata & 0xFF;
		if (rxdata & MX1_RXERR)
		{
			// Save error as out-of-band data which can be read via devctl()

			//dev->tty.oband_data |= rxdata;
			//atomic_set(&dev->tty.flags, OBAND_DATA);

			if (rxdata & MX1_URXD_BRK)
				key |= TTI_BREAK;
			else if (rxdata & MX1_URXD_FRMERR)
				key |= TTI_FRAME;
			else if (rxdata & MX1_URXD_PRERR)
				key |= TTI_PARITY;
			else if (rxdata & MX1_URXD_OVERRUN)
				key |= TTI_OVERRUN;
		}

		//printf("Serial port read:  %d %c\n", rxdata & 0xFF, rxdata & 0xFF);
		g_szSerialMsgTmp[g_iSerialMsgCur++] = rxdata&0xff;
		//status |= tti(&dev->tty, key);
		byte_count++;
	}

	// Note that as soon the RX FIFO data level drops below the RXTL threshold
	// the Receiver Ready (RRDY) interrupt will automatically clear


	// Clear the ageing timer interrupt if it caused this interrupt
	if (in32(base + MX1_UART_SR1) & MX1_USR1_AGTIM)
		out32(base + MX1_UART_SR1, MX1_USR1_AGTIM);

	/////////////////////////////////////////////////////////////////////////////
	//printf("Serial port read: %s\n", szTmp);

	arch_pte_restore(pageTableAddress);
	interrupt_forceschedule(iframe);

	return rsp;
}
#endif

/*
//static inline int rx_interrupt(DEV_MX1 *dev)
//{
//    return status;
//}
//

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

static size_t interrupt_serial(iframe_t *iframe){
	size_t rsp = (size_t)iframe;
	paddr_t pageTableAddress = arch_pte_backup();	//	__kernel_pml4);


	/////////////////////////////////////////////////////////////////////////////

	int            status = 0;
	int            byte_count = 0;
	unsigned       key, rxdata;
	uintptr_t      base = g_syspage_ptr->debug_dev.io_addr ;//	dev->base;

	//  limit loop iterations by FIFO size to prevent ISR from running too long
	while ((in32(base + MX1_UART_SR2) & MX1_USR2_RDR) && (byte_count < FIFO_SIZE))
	{
		// Read next character from FIFO

		rxdata = in32(base + MX1_UART_RXDATA);
		key = rxdata & 0xFF;
		if (rxdata & MX1_RXERR)
		{
			// Save error as out-of-band data which can be read via devctl()

			//dev->tty.oband_data |= rxdata;
			//atomic_set(&dev->tty.flags, OBAND_DATA);

			if (rxdata & MX1_URXD_BRK)
				key |= TTI_BREAK;
			else if (rxdata & MX1_URXD_FRMERR)
				key |= TTI_FRAME;
			else if (rxdata & MX1_URXD_PRERR)
				key |= TTI_PARITY;
			else if (rxdata & MX1_URXD_OVERRUN)
				key |= TTI_OVERRUN;
		}
		printf("Serial port read: %c\n", rxdata & 0xFF);
		//status |= tti(&dev->tty, key);
		byte_count++;
	}

	// Note that as soon the RX FIFO data level drops below the RXTL threshold
	// the Receiver Ready (RRDY) interrupt will automatically clear


	// Clear the ageing timer interrupt if it caused this interrupt
	if (in32(base + MX1_UART_SR1) & MX1_USR1_AGTIM)
		out32(base + MX1_UART_SR1, MX1_USR1_AGTIM);

	/////////////////////////////////////////////////////////////////////////////
	//	printf("Serial port read: %s\n", szTmp);

	arch_pte_restore(pageTableAddress);

	return rsp;
}
*/

/*
static size_t interrupt_serial(iframe_t *iframe){
	size_t rsp = (size_t)iframe;
	paddr_t pageTableAddress = arch_pte_backup();	//	EARLY_PML4_BASE);	//	__kernel_pml4);

	char 	szTmp[1024] = {0};
	int		iMsgCur		= 0;
	char	szMsgTmp	= '\0';

	while(inportb(0x3f8+5)&0x01)
	{
		szMsgTmp = inportb(0x3f8);	//readSerialPort();
		if(szMsgTmp==0) continue;
		szTmp[iMsgCur++] = szMsgTmp;
		if(iMsgCur>=1024)iMsgCur=0;
		//msgData.szMsg[iCur++] = szMsgTmp;	//	inportb(0x3f8);
		//if(iCur>=msgData.iMsgLen)iCur=0;
	}
	//	msgData.iMsgLen = iCur;

	printf("Serial port read: %s\n", szTmp);

	arch_pte_restore(pageTableAddress);

	return rsp;
}	*/


void interrupt_doubleError(iframe_t *iframe){	//	iframe_t* iframe){
    //	size_t rsp = (size_t)iframe;
	paddr_t pageTableAddress = arch_pte_backup();

	//	DBG("Double Error interrupt triggered with cs:%p ... \n", iframe->cs );	//	DBG("Timer interrupt triggered %d....\n", data->vector);

	arch_pte_restore(pageTableAddress);

	return;	// rsp;	//	return;
}

//External variable
extern volatile size_t gl_timer_tick;
bool start_schedule = false;
void interrupt_timer_schedule(iframe_t *iframe){	//	iframe_t* iframe){
    //	size_t rsp 				= (size_t)iframe;

	//	static int iTimeSlice 		= 0;
	int fNeedThreadSchedule			= 0;
	uint16_t cpuid = get_cpuvar()->cpu_id;
	THREAD_t*  pThreadSleepWakeUp = NULL;
	paddr_t pageTableAddress = arch_pte_backup();

	ack_irq(TIMER_IRQ_KERNEL);

	if(0 == cpuid) //Is bsp
	{
		barrier();
		p_vdso_data->ticks++;
		gl_timer_tick++ ;

		//	DBG("[%s:%d] gl_timer_tick:%ld \n",__func__,__LINE__, gl_timer_tick);
		barrier();
	}

	timeInterrupt(iframe,NULL);
	if (!start_schedule){
		arch_pte_restore(pageTableAddress);
		return;
	}

	//THREAD_t* pPrivilegeThread 	= NULL;
	//uint16_t cpuid = get_cpuvar()->cpu_id;


	//	printf("Timer interrupt triggered\n");//
	//	printf("Timer interrupt triggered ... rsp:%p rbp:%p \n", iframe->rsp, iframe->rbp);	//	DBG("Timer interrupt triggered %d....\n", data->vector);
	//	arch_pte_restore(pageTableAddress);

	//printf("Timer interrupt triggered ... rsp:%p rbp:%p \n", iframe->rsp, iframe->rsp);	//	DBG("Timer interrupt triggered %d....\n", data->vector);


	//pPrivilegeThread = thread_privilegequeue_pop();
	//if(pPrivilegeThread != NULL){
	//	start_sched_intr_privilege(pPrivilegeThread, iframe);
	//	//arch_pte_restore(pageTableAddress);
	//}else
	//if(start_schedule)
	{

		if(CURRENT_THREAD->state==THREAD_STATE_SIGWAITINFO){	//	after MSG_SEND, the thread status is changed to STATE_SIGWAITINFO
			fNeedThreadSchedule = 1;
		}

		if(CURRENT_THREAD->state==THREAD_STATE_SIGWAIT_INTREVENT){	// after wait for device interrupt, the thread status is changed to STATE_SIGWAIT_INTR
			fNeedThreadSchedule = 1;
		}

		//if(CURRENT_THREAD->state==THREAD_STATE_SIGWAIT_YIELD){	// after wait for device interrupt, the thread status is changed to STATE_SIGWAIT_INTR
		//	iSetToWait = 1;
		//}

		if(CURRENT_THREAD->state==THREAD_STATE_NANOSLEEP){	// after wait for device interrupt, the thread status is changed to STATE_SIGWAIT_INTR
			fNeedThreadSchedule = 1;
		}

		if(CURRENT_THREAD->state==THREAD_STATE_EXIT){	// after wait for device interrupt, the thread status is changed to STATE_SIGWAIT_INTR
			fNeedThreadSchedule = 1;
		}


		if(CURRENT_THREAD->schedulePolicy!=SCHEDULE_POLICY_FIFO){
			pThreadSleepWakeUp = sleep_wait_wakeup_lookup_lists_update_remove();
		}else{
			sleep_wait_wakeup_lookup_lists_update_only();
		}

		if(pThreadSleepWakeUp!=NULL){
			start_sched_intr(iframe, 1, pThreadSleepWakeUp);
		}
		else if(fNeedThreadSchedule
				|| ((CURRENT_THREAD->schedulePolicy!=SCHEDULE_POLICY_FIFO)&&(CURRENT_THREAD->quantum++>=THREAD_SCHEDULE_QUANTUM )))	//if(iSetToWait || (iTimeSlice++>20))	//	10))	//20))	//000	//00)
		{
			// KDBG("Timer interrupt triggered with cs:%p ss:%p rflag:%p rip:%p rsp:%p rbp:%p... \n",
			// 	iframe->cs, iframe->ss, iframe->rflags, iframe->rip, iframe->rsp, iframe->rbp );
			//iTimeSlice = 0;

			CURRENT_THREAD->quantum = 0;
			start_sched_intr(iframe, 1, NULL);	//	taskmgr_schedule();

			//arch_pte_restore(pageTableAddress);
		}else{
			arch_pte_restore(pageTableAddress);
		}
	}


	return;	// rsp;	//	return;
}

void init_taskmgr(syspage_t* syspage_ptr){	//	ky_syspage *sysp){
	//uint32_t irqnum = syspage_ptr->systime.intr;
	//int		iDevID	= 48;
	//uint64_t flag = flag & ( ~(0x1));

	//register_interrupt_handler_test(irqnum, interrupt_timer_schedule3);
	register_interrupt_handler(DOUBLEERR_IRQ_KERNEL, interrupt_doubleError);

	//register_interrupt_handler2(0, interrupt_timer_schedule2, &iDevID, NULL, flag);
	register_interrupt_handler(TIMER_IRQ_KERNEL, interrupt_timer_schedule);	//	register_interrupt_handler(irqnum, interrupt_timer_schedule);	//	register_interrupt_handler((VECTOR_IRQ_BASE + TIMER_IRQ), interrupt_timer_schedule3);//apic_timer_irq_handler);

	register_interrupt_handler(KEYBOARD_IRQ_KERNEL, interrupt_forceschedule);	//	For preemptive schedule		//interrupt_keyboard);
//#ifdef ARCH_ARM
//	register_interrupt_handler(syspage_ptr->devices[0]->irq_num, interrupt_serial);			//	interrupt_forceschedule);	//	interrupt_forceschedule);
//#else
	register_interrupt_handler(syspage_ptr->devices[0]->irq_num, interrupt_forceschedule);
	
//#endif
	register_interrupt_handler(88, interrupt_forceschedule);

	//	register_interrupt_handler2(0, interrupt_timer_schedule2, &iDevID, NULL, flag);
	//	register_interrupt_handler2(1, interrupt_keyboard, &iDevID, NULL, flag);
	//	register_interrupt_handler2(4, interrupt_serial, &iDevID, NULL, flag);
	//	register_interrupt_handler2(12, interrupt_mouse, &iDevID, NULL, flag);
}
