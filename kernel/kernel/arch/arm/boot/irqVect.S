	
.text
.global   _vector_table, clean_bss
.section ".vectors", "ax"
_vector_table:
	ldr 	pc, =Reset_Handler			 /* Reset				   */
	ldr 	pc, =Undefined_Handler		 /* Undefined instructions */
	ldr 	pc, =SVC_Handler			 /* Supervisor Call 	   */
	b halt//ldr 	pc, =PrefAbort_Handler		 /* Prefetch abort		   */
	b halt//ldr 	pc, =DataAbort_Handler		 /* Data abort			   */
	.word	0							 /* RESERVED			   */
	ldr 	pc, =IRQ_Handler			 /* IRQ interrupt		   */
	b halt//ldr 	pc, =FIQ_Handler			 /* FIQ interrupt		   */

.align 2
Undefined_Handler:
	/* ִ�е�����֮ǰ:
	 * 1. lr_und�����б��ж�ģʽ�е���һ������ִ�е�ָ��ĵ�ַ
	 * 2. SPSR_und�����б��ж�ģʽ��CPSR
	 * 3. CPSR�е�M4-M0������Ϊ11011, ���뵽undģʽ
	 * 4. ����0x4�ĵط�ִ�г��� 
	 */

	/* ��und�쳣�����������п��ܻ��޸�r0-r12, �����ȱ��� */
	/* lr���쳣�������ķ��ص�ַ, ҲҪ���� */
	stmdb sp!, {r0-r12, lr}  
	
	/* �����ֳ� */
	/* ����und�쳣 */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* �ָ��ֳ� */
	ldmia sp!, {r0-r12, pc}^  /* ^���spsr��ֵ�ָ���cpsr�� */
	
und_string:
	.string "undefined instruction exception"

.align 2
SVC_Handler:
	/* ִ�е�����֮ǰ:
	 * 1. lr_svc�����б��ж�ģʽ�е���һ������ִ�е�ָ��ĵ�ַ
	 * 2. SPSR_svc�����б��ж�ģʽ��CPSR
	 * 3. CPSR�е�M4-M0������Ϊ10011, ���뵽svcģʽ
	 * 4. ����0x08�ĵط�ִ�г��� 
	 */

	/* �����ֳ� */
	/* ��swi�쳣�����������п��ܻ��޸�r0-r12, �����ȱ��� */
	/* lr���쳣�������ķ��ص�ַ, ҲҪ���� */
	stmdb sp!, {r0-r12, lr}  

	mov r4, lr
	
	/* ����swi�쳣 */
	mrs r0, cpsr
	ldr r1, =swi_string
	bl printException

	sub r0, r4, #4
	bl printSWIVal
	
	/* �ָ��ֳ� */
	ldmia sp!, {r0-r12, pc}^  /* ^���spsr��ֵ�ָ���cpsr�� */
	
swi_string:
	.string "swi exception"

.align 2


/* IRQ�жϣ��ص㣡�������� */
IRQ_Handler:
	push {lr}					/* ����lr��ַ */
	push {r0-r3, r12}			/* ����r0-r3��r12�Ĵ��� */

	mrs r0, spsr				/* ��ȡspsr�Ĵ��� */
	push {r0}					/* ����spsr�Ĵ��� */

	mrc p15, 4, r1, c15, c0, 0 /* ��CP15��C0�Ĵ����ڵ�ֵ��R1�Ĵ�����
								* �ο��ĵ�ARM Cortex-A(armV7)����ֲ�V4.0.pdf P49
								* Cortex-A7 Technical ReferenceManua.pdf P68 P138
								*/							
	add r1, r1, #0x0100			/* GIC����ַ��0x0100��Ҳ����GIC��CPU�ӿڶ˻���ַ */
	ldr r0, [r1, #0xC]			/* GIC��CPU�ӿڶ˻���ַ��0X0C����GICC_IAR�Ĵ�����
								 * GICC_IAR�Ĵ��������⵱ǰ�����жϵ��жϺţ�����Ҫ����
								 * ����жϺ������Ե����ĸ��жϷ�����
								 */
	push {r0, r1}				/* ����r0,r1 */
	
	cps #0x13					/* ����SVCģʽ�����������ж��ٴν�ȥ */
	
	push {lr}					/* ����SVCģʽ��lr�Ĵ��� */
	ldr r2, =handle_irq_c	/* ����C�����жϴ���������r2�Ĵ�����*/
	blx r2						/* ����C�����жϴ�������������һ��������������R0�Ĵ����� */

	pop {lr}					/* ִ����C�����жϷ�������lr��ջ */
	cps #0x12					/* ����IRQģʽ */
	pop {r0, r1}				
	str r0, [r1, #0X10]			/* �ж�ִ����ɣ�дEOIR */

	pop {r0}						
	msr spsr_cxsf, r0			/* �ָ�spsr */

	pop {r0-r3, r12}			/* r0-r3,r12��ջ */
	pop {lr}					/* lr��ջ */
	subs pc, lr, #4				/* ��lr-4����pc */
	


IRQ_Handler2:
	/* ִ�е�����֮ǰ:
	 * 1. lr_irq�����б��ж�ģʽ�е���һ������ִ�е�ָ��ĵ�ַ
	 * 2. SPSR_irq�����б��ж�ģʽ��CPSR
	 * 3. CPSR�е�M4-M0������Ϊ10010, ���뵽irqģʽ
	 * 4. ����0x18�ĵط�ִ�г��� 
	 */



	/* �����ֳ� */
	/* ��irq�쳣�����������п��ܻ��޸�r0-r12, �����ȱ��� */
	/* lr-4���쳣�������ķ��ص�ַ, ҲҪ���� */
    /*
	sub lr, lr, #4
	stmdb sp!, {r0-r12, lr}  

	*/

	
	/* ����irq�쳣 */
    /*
	bl handle_irq_c
	*/
	/* �ָ��ֳ� */
    /*
	ldmia sp!, {r0-r12, pc}^    ^���spsr_irq��ֵ�ָ���cpsr�� */	


.align 2
Reset_Handler:
	/* Reset SCTlr Settings */
	mrc 	p15, 0, r0, c1, c0, 0	  /* read SCTRL, Read CP15 System Control register		*/
	bic 	r0,  r0, #(0x1 << 13)	  /* Clear V bit 13 to use normal exception vectors  	*/
	bic 	r0,  r0, #(0x1 << 12)	  /* Clear I bit 12 to disable I Cache					*/
	bic 	r0,  r0, #(0x1 <<  2)	  /* Clear C bit  2 to disable D Cache					*/
	bic 	r0,  r0, #(0x1 << 2)	  /* Clear A bit  1 to disable strict alignment 		*/
	bic 	r0,  r0, #(0x1 << 11)	  /* Clear Z bit 11 to disable branch prediction		*/
	bic 	r0,  r0, #0x1			  /* Clear M bit  0 to disable MMU						*/
	mcr 	p15, 0, r0, c1, c0, 0	  /* write SCTRL, Write to CP15 System Control register	*/

    cps     #0x1B                /* Enter undef mode                */
  /*  ldr     sp, =__boot_stack_base + 0x1000*5     Set up undef mode stack      */
    ldr     sp, =stackUND
   
    cps     #0x12                /* Enter irq mode                */
 /*   ldr     sp, =__boot_stack_base + 0x1000*3    Set up irq mode stack      */
    ldr sp, =stackIRQ
  
    cps     #0x13                /* Enter Supervisor mode         */
   /* ldr     sp, =__boot_stack_base + 0x1000*1    Set up Supervisor Mode stack  */
    ldr sp, =stackSVC

	ldr r0, =_vector_table
	mcr p15, 0, r0, c12, c0, 0  /* set VBAR, Vector Base Address Register*/
	//mrc p15, 0, r0, c12, c0, 0  //read VBAR

	bl clean_bss

/*	bl system_init */
	cpsie	i					 /* Unmask interrupts			  */

    bl bootstrap_core_init

halt:
	b  halt




clean_bss:
	/* ���BSS�� */
	ldr r1, =__bss
	ldr r2, =__bss_end
	mov r3, #0
clean:
	cmp r1, r2
	strlt r3, [r1]
	add r1, r1, #4
	blt clean
	
	mov pc, lr