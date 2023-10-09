
#ifndef __DEBUG_DEV_H__
#define __DEBUG_DEV_H__

// #define WIKA_BOARD
// #define DIGI_BOARD
#ifdef WIKA_BOARD
 #define WATCHDOG_DEV_BASE 0x0209C000
 #define DEBUG_DEV_BASE 0x02020000	//	MX6X_UART4_BASE
 #define DEBUG_DEV_IRQ  58			//	UART4_IRQn
#else
#ifdef DIGI_BOARD
 #define DEBUG_DEV_BASE 0x021F0000	//	MX6X_UART4_BASE
 #define DEBUG_DEV_IRQ  61			//	UART4_IRQn
#else
 #define DEBUG_DEV_BASE 0x021E8000	//	MX6X_UART2_BASE
 #define DEBUG_DEV_IRQ  59			//	UART2_IRQn
#endif
#endif


#endif
