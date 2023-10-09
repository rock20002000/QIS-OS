	
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
	/* 执行到这里之前:
	 * 1. lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  
	
	/* 保存现场 */
	/* 处理und异常 */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
und_string:
	.string "undefined instruction exception"

.align 2
SVC_Handler:
	/* 执行到这里之前:
	 * 1. lr_svc保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_svc保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10011, 进入到svc模式
	 * 4. 跳到0x08的地方执行程序 
	 */

	/* 保存现场 */
	/* 在swi异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  

	mov r4, lr
	
	/* 处理swi异常 */
	mrs r0, cpsr
	ldr r1, =swi_string
	bl printException

	sub r0, r4, #4
	bl printSWIVal
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
swi_string:
	.string "swi exception"

.align 2


/* IRQ中断！重点！！！！！ */
IRQ_Handler:
	push {lr}					/* 保存lr地址 */
	push {r0-r3, r12}			/* 保存r0-r3，r12寄存器 */

	mrs r0, spsr				/* 读取spsr寄存器 */
	push {r0}					/* 保存spsr寄存器 */

	mrc p15, 4, r1, c15, c0, 0 /* 从CP15的C0寄存器内的值到R1寄存器中
								* 参考文档ARM Cortex-A(armV7)编程手册V4.0.pdf P49
								* Cortex-A7 Technical ReferenceManua.pdf P68 P138
								*/							
	add r1, r1, #0x0100			/* GIC基地址加0x0100，也就是GIC的CPU接口端基地址 */
	ldr r0, [r1, #0xC]			/* GIC的CPU接口端基地址加0X0C就是GICC_IAR寄存器，
								 * GICC_IAR寄存器保存这当前发生中断的中断号，我们要根据
								 * 这个中断号来绝对调用哪个中断服务函数
								 */
	push {r0, r1}				/* 保存r0,r1 */
	
	cps #0x13					/* 进入SVC模式，允许其他中断再次进去 */
	
	push {lr}					/* 保存SVC模式的lr寄存器 */
	ldr r2, =handle_irq_c	/* 加载C语言中断处理函数到r2寄存器中*/
	blx r2						/* 运行C语言中断处理函数，带有一个参数，保存在R0寄存器中 */

	pop {lr}					/* 执行完C语言中断服务函数，lr出栈 */
	cps #0x12					/* 进入IRQ模式 */
	pop {r0, r1}				
	str r0, [r1, #0X10]			/* 中断执行完成，写EOIR */

	pop {r0}						
	msr spsr_cxsf, r0			/* 恢复spsr */

	pop {r0-r3, r12}			/* r0-r3,r12出栈 */
	pop {lr}					/* lr出栈 */
	subs pc, lr, #4				/* 将lr-4赋给pc */
	


IRQ_Handler2:
	/* 执行到这里之前:
	 * 1. lr_irq保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_irq保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10010, 进入到irq模式
	 * 4. 跳到0x18的地方执行程序 
	 */



	/* 保存现场 */
	/* 在irq异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr-4是异常处理完后的返回地址, 也要保存 */
    /*
	sub lr, lr, #4
	stmdb sp!, {r0-r12, lr}  

	*/

	
	/* 处理irq异常 */
    /*
	bl handle_irq_c
	*/
	/* 恢复现场 */
    /*
	ldmia sp!, {r0-r12, pc}^    ^会把spsr_irq的值恢复到cpsr里 */	


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
	/* 清除BSS段 */
	ldr r1, =__bss
	ldr r2, =__bss_end
	mov r3, #0
clean:
	cmp r1, r2
	strlt r3, [r1]
	add r1, r1, #4
	blt clean
	
	mov pc, lr