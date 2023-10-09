.TEXT  
.type smp_spin_pl310, function
.GLOBL smp_spin_pl310
.cfi_startproc

/*
    void  smp_spin_pl310(&smp, 0)
    r0 contains the smp_entry_t* smp
    r1 contain the  cpu index
*/
smp_spin_pl310:
    /*Prologue*/
    push {r2, r3, r4, r5, r11, ip}
    add r11, sp, #0
    sub sp,  sp, #0x14
    /*
        Set smp.pending to 0  to tell transfer_aps()
        to start the next processer
    */

    ldr  ip,  =0x00A02000
    
    lsl r5, r1, #4
    add r3, r0, r5 // r3 == smp[r1]
    mov r2, #0
    str r2, [r3, #0]
    NOP

    /*
        CPU0 is still running with MMU/Caches disabled
        So we need to flush the data cache line for  smp.pending
        So cpu0 see th site.

        Warning : assumes cpu supports clean-dcache-by MVA 
                 and 32-byte line.
    */
    bic r2, r3, #0x1f
    mov r4, #0
    dsb 
    mcr p15,0, r2, c7, c10, 1 //clean dcache line by MVA
    mcr p15,0, r4, c7, c10, 4 //Data synchronisation barrier

    /*
        Clean L2 Line to Memory So cpu0 sees the write
    */
    mov r4, r2, asl #20
    mov r4, r4, lsr #20  //Page offset
    mcr p15, 0, r2, c7,c8, 0 //page offset
    mrc p15, 0, r2, c7,c4, 0 // VA2PA for privileged read access
    bic r2, r2,  #0xff
    bic r2, r2,  #0xf00
    orr r2, r2,  r4        //add page offset
    str r2, [ip, #0x7b0]   //clean line by PA
    mov r2, #0
    str r2, [ip, #0x730]   //cache Sync
0:  ldr r2, [ip, #0x730]
    teq r2, #0
    bne 0b

    /*
        Wait until smp.start_addr is non-zero then
        Atomically Exchange it with 0
        So only this Cpu comes in out of the Loop
    */
    add r2, r3, #4  //smp.start
0:  ldr ip, [r2]
    teq ip, #0
    beq 0b
    ldrex ip, [r2]
    strex r5, r4, [r2]
    teq r5, #1
    teqne ip, #0
    beq 0b   //strex failed or start_addr == 0
    NOP
    mcr p15, 0, r4, c7, c10, 5 //Data Memory barrier

    mvn r4, #0
    teq ip, r4
    movne pc, ip //Jump to start_addr

    /*
        If start_addr is -1, Kernel wants us to shutdown
        Stuff the -1 back into start_addr to shutdown the next processor
    */
    str ip, [r2]
    mov r0, #0

    0: dsb
       wfi
       b  0b

 /*Epilogue*/
    add sp, r11, #0
    pop {r2, r3, r4, r5, r11, ip}
    bx lr

.cfi_endproc

.text
.GLOBL sendSGI
.type sendSGI, function

/*
    void sendSGI(uint32_t id, uint32_t target_list, uint32_t filter_list )
    Send a software generate Interrupt
*/

.cfi_startproc
    and  r3, r0, #0x0F  // Mask off unused bits of ID, and move to r3
    and  r1, r1, #0x0f  // Mask off unused bit of  target_filer
    and  r2, r2, #0x0f  //Mask off unused bits of filter_list

    orr r3, r3, r1, LSL #16  //Comine ID and target_filter
    orr r3, r3, r2, LSL #24  //and now the filerf list

    //Get the address of the GIC
    mrc p15, 4, r0, c15, c0, 0 //Read periph base address
    add r0, r0, #0x1F00 // Add offsert of the sgi_trigger reg

    str r3,  [r0]  //Write to the Software Generated Interrupt Register(ICDGIR)
    BX lr 
 
.cfi_endproc
