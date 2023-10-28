.section ".text", "ax"

# Will jump to entry address and never return;

.global run_next

run_next:
    # The boot info address is stored in r0 and bsp entry is stored in r1
   
    # Just set pc = r1 to enter bsp code

    /*
        * Call entry_point(prt_bootinfo)
    */
    mov		pc, r1

