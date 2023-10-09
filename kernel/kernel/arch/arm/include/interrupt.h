#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>
/// The interrupt stack frame.
struct iframe {
	uint32_t sp;
	uint32_t lr;
	uint32_t spsr;

	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;

	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;	//	bp
	uint32_t r10;	//	sl
	uint32_t r11;	//	fp
	uint32_t r12;	//	ip

	uint32_t pc;	//	lr;
};


/*struct iframe {
	uint32_t spsr;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;

	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;	//	bp
	uint32_t r10;	//	sl
	uint32_t r11;	//	fp


	uint32_t r12;	//	ip
	uint32_t pc;	//	lr;

	//	uint32_t r12;	//	ip
	//uint32_t r13;	//	sp
	//uint32_t r14;	//	lr
	//uint32_t r15;	//	pc
	uint32_t sp;
	//uint32_t lr;
	uint32_t lr;	//	pc;
};	*/

typedef struct iframe iframe_t;

typedef void (*isr_t)(iframe_t*);

struct sigevent;

void init_irq_handler(void);
void register_interrupt_handler(uint8_t vec, isr_t handler);
void arm_handle_interrupt(unsigned int iIrqVec, iframe_t *frame);

void gen_softintr();

uint32_t get_gic_base(void);

long arm_handle_syscall(iframe_t* frame);
long handle_syscall(iframe_t* iframe);			//, long a1, long a2, long a3, long a4, long a5);

int enqueueEventPulse(struct sigevent* event);
int createTimerCallBackfunction(void* arg);




int enqueueEventPulse(struct sigevent* event);
int createTimerCallBackfunction(void *arg);

extern volatile uint8_t phy_interrupt_nest ;
extern void phy_hw_interrupt_enable(phy_base_t level);
extern phy_base_t phy_hw_interrupt_disable();

//static void phy_interrupt_enter(void)
//{
//    phy_base_t level;
//
//    level = phy_hw_interrupt_disable();
//    phy_interrupt_nest ++;
//
//    phy_hw_interrupt_enable(level);
//
//}
//
//static void phy_interrupt_leave(void)
//{
//    phy_base_t level;
//    level = phy_hw_interrupt_disable();
//    phy_interrupt_nest --;
//    phy_hw_interrupt_enable(level);
//}



#endif
