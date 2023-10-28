#include <platform/include/platform.h>
#include "epit.h"
#include "stdint.h"

GIC_V2_Type* get_gic_base_vaddr(void)
{
	GIC_V2_Type *dst;
    paddr_t dst_paddr =0x00a00000;

	//__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst_paddr));
    //printf("GIC Base:0x%p\n", dst);
    dst = (GIC_V2_Type *)mmap_device_io(0x4000, dst_paddr);
	return dst;
}

GIC_Type *gic = NULL;

EPIT_Type *	g_Epit1VAddr = NULL;


void epit1_irqhandler()	//uint32_t giccIar, void *param )
{
	int nr;

     xprintf(" [%s:%d] \n",__func__, __LINE__ );

	//gic = get_gic_base_vaddr();
	/* The processor reads GICC_IAR to obtain the interrupt ID of the
	 * signaled interrupt. This read acts as an acknowledge for the interrupt
	 */
//	nr = gic->C_IAR;
	nr = 88;	//giccIar & 0x3FFUL;
    
    if(g_Epit1VAddr->SR & (1<<0)) 	//	if(EPIT1->SR & (1<<0))
	    xprintf("irq %d is happened EPIT1->SR: %d\r\n", nr, g_Epit1VAddr->SR);
	
	//irq_table[nr].irq_handler(nr, irq_table[nr].param);

	/* write GICC_EOIR inform the CPU interface that it has completed 
	 * the processing of the specified interrupt 
	 */
	gic->C_EOIR = nr;
	g_Epit1VAddr->SR |= 1<<0;	//	EPIT1->SR |= 1<<0; 				/* ����жϱ�־λ */
}

void initEpitVaddr(){
    gic = get_gic_base_vaddr();
	g_Epit1VAddr = (EPIT_Type *)mmap_device_io(0x1000, (paddr_t)EPIT1_BASE);	//	UART2_PORT);
}

FORCEDINLINE __STATIC_INLINE void GIC_EnableIRQ_User(uint32_t IRQn)
{
  //	paddr_t gicPaddr = (paddr_t)(__get_CBAR() & 0xFFFF0000UL);
   // paddr_t gicPaddr = 0x00a00000;
    xprintf(" [%s:%d] \n",__func__, __LINE__ );
  //  GIC_V2_Type* gic = (GIC_V2_Type *)mmap_device_io(0x1000, gicPaddr );
  	gic->D_ISENABLER[((uint32_t)(int32_t)IRQn) >> 5] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

/* assume use ipc clk which is 66MHz, 1us against to 66 count */
#define NS_TO_COUNT(ns) (ns*1000/66)

void epit1_init(unsigned int frac, unsigned int value)
{
	if(frac > 0XFFF) 	frac = 0XFFF;
 xprintf(" [%s:%d] \n",__func__, __LINE__ );
    initEpitVaddr();        
		 xprintf(" [%s:%d] \n",__func__, __LINE__ );
	g_Epit1VAddr->CR = 0;	/* ������CR�Ĵ��� */
	
	/*
     * CR�Ĵ���:
     * bit25:24 01 ʱ��Դѡ��Peripheral clock=66MHz
     * bit15:4  frac ��Ƶֵ
     * bit3:	1  ����������0�Ļ���LR���¼�����ֵ
     * bit2:	1  �Ƚ��ж�ʹ��
     * bit1:    1  ��ʼ����ֵ��Դ��LR�Ĵ���ֵ
     * bit0:    0  �ȹر�EPIT1
     */
	g_Epit1VAddr->CR = (1<<24 | frac << 4 | 1<<3 | 1<<2 | 1<<1);
	
	g_Epit1VAddr->LR = value;	/* ������ֵ */
	g_Epit1VAddr->CMPR	= 0;	/* �ȽϼĴ�������������ֵ�ʹ˼Ĵ���ֵ��ȵĻ��ͻ�����ж� */
 xprintf(" [%s:%d] \n",__func__, __LINE__ );
	/* ʹ��GIC�ж�Ӧ���ж� 			*/
	//	GIC_EnableIRQ_User(EPIT1_IRQn);
xprintf(" [%s:%d] \n",__func__, __LINE__ );
	/* ע���жϷ����� 	
    request_irq(IRQn_Type irq, irq_handler_t handler, void *param)
    		*/
	//request_irq(EPIT1_IRQn, (irq_handler_t)epit1_irqhandler, NULL); 
     //AttachInterruptC(EPIT1_IRQn, (start_rtn)epit1_irqhandler);
xprintf(" [%s:%d] g_Epit1VAddr->CR:0x%p \n",__func__, __LINE__,g_Epit1VAddr->CR );
	g_Epit1VAddr->CR |= 1<<0;	/* ʹ��EPIT1 */ 
xprintf(" [%s:%d] g_Epit1VAddr->CR:0x%p \n",__func__, __LINE__,g_Epit1VAddr->CR );
}

int epit1Open()   {	return 0; }
int epit1Read()   {	return 0; }
int epit1Write()  {	return 0; }
int epit1Close()  {	return 0; }

void main(void)
{
    int iStatus 	= 0;
    int devToken 	= 'e'<<8 | 'p' ;

    MSG_DATA 	msgData;
    MSG_CONTEXT msgContext;

     int			i 		= 0;
    int			iMsgLen = 0;
    int			iRslt	= 0;

    char		szMsgTmp = '\0';

    DEVICE_FUNCTIONS devFunctions;

    int	iDevID	= 88;
    char*  wx="param pass to INT";

    xprintf(" [%s:%d] \n",__func__, __LINE__ );

    devFunctions.devopen 	= epit1Open;
    devFunctions.devread 	= epit1Read;
    devFunctions.devwrite 	= epit1Write;
    devFunctions.devclose 	= epit1Close;
     xprintf(" [%s:%d] \n",__func__, __LINE__ );
    

    int fdDriver = devRegister(&iStatus, "/dev/epit1", devToken, &devFunctions);
    xprintf("Test driver registered with index: %d\n", fdDriver);

    AttachInterruptC(EPIT1_IRQn, (start_rtn)epit1_irqhandler);	//	user_int_register(int_handler_user, 4, &iDevID, (void*)wx, 0, 0);
    
    epit1_init(0, NS_TO_COUNT(1000000));
    while(1){
         // xprintf(" [%s:%d] \n",__func__, __LINE__ );
    	iRslt = devMsgRecvC(fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fd, &msgData, sizeof(MSG_DATA), &msgContext, 0);	//	fdDriver, &msgData, sizeof(MSG_DATA), &msgContext, 0);
        //xprintf(" [%s:%d] iRslt: %d\n",__func__, __LINE__,iRslt );
    	if(iRslt<=0) continue;
    }
}
