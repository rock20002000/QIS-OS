#ifndef   __UART_H__
#define   __UART_H__

#include "common.h"
#include <platform/arch/arm/include/mx6q/mx6x_iomux.h>
#include <platform/arch/arm/include/mx6q/mx6x.h>

#define MX6Q_PAD_SETTINGS_UART (PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE | \
                				PAD_CTL_PUS_100K_PU | PAD_CTL_SPEED_MEDIUM | \
                                PAD_CTL_DSE_40_OHM | PAD_CTL_SRE_FAST | PAD_CTL_PUE_PULL)


/*
 * These are the new stuffs for generic serial image download.
 * Use init_serdev() to register device specific character
 * get/put and poll(option) functions, after this, generic
 * image_download(), ser_getchar(), ser_putchar(), ser_putstr(),
 * ser_puthex() and ser_poll() will function properly.
 * NOTE: you must call init_serxxx() before calling any of those
 * functions.
 */

typedef struct _ser_dev_t {
	unsigned char	(*get_byte)(void);
	void			(*put_byte)(unsigned char);
	unsigned char	(*poll)(void);
} ser_dev;

void putchar(int c);
int getchar(void);
void puts(const char *s);

extern void				init_serdev(ser_dev *);
extern unsigned			image_download_ser(unsigned);
extern unsigned char	ser_getchar(void);
extern unsigned char	ser_poll(void);
extern void				ser_putchar(char);
extern void				ser_putstr(const char *);
extern void				ser_puthex(unsigned);
extern void				ser_puthex8(unsigned);
extern void				ser_puthex16(unsigned);
extern void				ser_puthex32(unsigned);
extern void				ser_putdec(unsigned);


#endif
