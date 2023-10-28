#ifndef __ARCH_SYSCALL_H__
#define __ARCH_SYSCALL_H__

#include <types.h>

static inline long syscall(int n, long a1, long a2, long a3, long a4, long a5) {
	long ret;
    register long r0 __asm__("r0") = n;
    register long r1 __asm__("r1") = a1;
    register long r2 __asm__("r2") = a2;
    register long r3 __asm__("r3") = a3;
    register long r4 __asm__("r4") = a4;
    register long r5 __asm__("r5") = a5;
	switch(n){
	case 0:
		__asm__ __volatile__("swi 0 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 1:
		__asm__ __volatile__("swi 1 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 2:
		__asm__ __volatile__("swi 2 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 3:
		__asm__ __volatile__("swi 3 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 4:
		__asm__ __volatile__("swi 4 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 5:
		__asm__ __volatile__("swi 5 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 6:
		__asm__ __volatile__("swi 6 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 7:
		__asm__ __volatile__("swi 7 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 8:
		__asm__ __volatile__("swi 8 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 9:
		__asm__ __volatile__("swi 9 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 10:
		__asm__ __volatile__("swi 10 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 11:
		__asm__ __volatile__("swi 11 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 12:
		__asm__ __volatile__("swi 12 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 13:
		__asm__ __volatile__("swi 13 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 14:
		__asm__ __volatile__("swi 14 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 15:
		__asm__ __volatile__("swi 15 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 16:
		__asm__ __volatile__("swi 16 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 17:
		__asm__ __volatile__("swi 17 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 18:
		__asm__ __volatile__("swi 18 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 19:
		__asm__ __volatile__("swi 19 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 20:
		__asm__ __volatile__("swi 20 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 21:
		__asm__ __volatile__("swi 21 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 22:
		__asm__ __volatile__("swi 22 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 23:
		__asm__ __volatile__("swi 23 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 24:
		__asm__ __volatile__("swi 24 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 25:
		__asm__ __volatile__("swi 25 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 26:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 26 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");
		break;
	case 27:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 27 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");
		break;
	case 28:
		__asm__ __volatile__("swi 28 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 29:
		__asm__ __volatile__("swi 29 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 30:
		__asm__ __volatile__("swi 30 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 31:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 31 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");

		/*	__asm__ __volatile__("stmfd sp!, {r4-r12, lr}");
		__asm__ __volatile__("mov r6, sp");
		__asm__ __volatile__("swi 31 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		__asm__ __volatile__("mov sp, r6");
		__asm__ __volatile__("ldmfd sp!, {r4-r12, lr}");	*/
		break;
	case 32:
		__asm__ __volatile__("swi 32 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 33:
		__asm__ __volatile__("swi 33 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 34:
		__asm__ __volatile__("swi 34 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 35:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 35 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");

		break;
	case 36:
		__asm__ __volatile__("swi 36 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 37:
		__asm__ __volatile__("swi 37 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 38:
		__asm__ __volatile__("swi 38 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 39:
		__asm__ __volatile__("swi 39 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 40:
		__asm__ __volatile__("swi 40 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 41:
		__asm__ __volatile__("swi 41 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 42:
		__asm__ __volatile__("swi 42 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 43:
		__asm__ __volatile__("swi 43 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 44:
		__asm__ __volatile__("swi 44 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 45:
		__asm__ __volatile__("swi 45 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 46:
		__asm__ __volatile__("swi 46 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 47:
		__asm__ __volatile__("swi 47 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 48:
		__asm__ __volatile__("swi 48 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 49:
		__asm__ __volatile__("swi 49 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 50:
		__asm__ __volatile__("swi 50 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 51:
		__asm__ __volatile__("swi 51 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 52:
		__asm__ __volatile__("swi 52 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 53:
		__asm__ __volatile__("swi 53 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 54:
		__asm__ __volatile__("swi 54 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 55:
		__asm__ __volatile__("swi 55 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 56:
		__asm__ __volatile__("swi 56 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 57:
		__asm__ __volatile__("swi 57 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 58:
		__asm__ __volatile__("swi 58 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 59:
		__asm__ __volatile__("swi 59 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 60:
		__asm__ __volatile__("swi 60 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 61:
		__asm__ __volatile__("swi 61 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 62:
		__asm__ __volatile__("swi 62 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 63:
		__asm__ __volatile__("swi 63 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 64:
		__asm__ __volatile__("swi 64 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 65:
		__asm__ __volatile__("swi 65 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 66:
		__asm__ __volatile__("swi 66 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 67:
		__asm__ __volatile__("swi 67 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 68:
		__asm__ __volatile__("swi 68 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 69:
		__asm__ __volatile__("swi 69 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 70:
		__asm__ __volatile__("swi 70 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 71:
		__asm__ __volatile__("swi 71 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 72:
		__asm__ __volatile__("swi 72 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 73:
		__asm__ __volatile__("swi 73 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 74:
		__asm__ __volatile__("swi 74 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 75:
		__asm__ __volatile__("swi 75 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 76:
		__asm__ __volatile__("swi 76 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 77:
		__asm__ __volatile__("swi 77 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 78:
		__asm__ __volatile__("swi 78 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 79:
		__asm__ __volatile__("swi 79 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 80:
		__asm__ __volatile__("swi 80 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 81:
		__asm__ __volatile__("swi 81 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 82:
		__asm__ __volatile__("swi 82 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 83:
		__asm__ __volatile__("swi 83 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 84:
		__asm__ __volatile__("swi 84 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 85:
		__asm__ __volatile__("swi 85 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 86:
		__asm__ __volatile__("swi 86 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 87:
		__asm__ __volatile__("swi 87 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 88:
		__asm__ __volatile__("swi 88 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 89:
		__asm__ __volatile__("swi 89 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 90:
		__asm__ __volatile__("swi 90 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 91:
		__asm__ __volatile__("swi 91 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 92:
		__asm__ __volatile__("swi 92 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 93:
		__asm__ __volatile__("swi 93 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 94:
		__asm__ __volatile__("swi 94 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 95:
		__asm__ __volatile__("swi 95 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 96:
		__asm__ __volatile__("swi 96 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 97:
		__asm__ __volatile__("swi 97 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 98:
		__asm__ __volatile__("swi 98 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 99:
		__asm__ __volatile__("swi 99 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 100:
		__asm__ __volatile__("swi 100 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 101:
		__asm__ __volatile__("swi 101 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 102:
		__asm__ __volatile__("swi 102 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 103:
		__asm__ __volatile__("swi 103 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 104:
		__asm__ __volatile__("swi 104 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 105:
		__asm__ __volatile__("swi 105 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 106:
		__asm__ __volatile__("swi 106 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 107:
		__asm__ __volatile__("swi 107 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 108:
		__asm__ __volatile__("swi 108 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 109:
		__asm__ __volatile__("swi 109 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 110:
		__asm__ __volatile__("swi 110 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 111:
		__asm__ __volatile__("swi 111 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 112:
		__asm__ __volatile__("swi 112 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 113:
		__asm__ __volatile__("swi 113 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 114:
		__asm__ __volatile__("swi 114 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 115:
		__asm__ __volatile__("swi 115 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 116:
		__asm__ __volatile__("swi 116 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 117:
		__asm__ __volatile__("swi 117 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 118:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 118 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");
		break;
	case 119:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 119 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");
		break;
	case 120:
		__asm__ __volatile__("swi 120 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 121:
		__asm__ __volatile__("swi 121 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 122:
		__asm__ __volatile__("swi 122 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 123:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 123 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");

		/*__asm__ __volatile__("stmfd sp!, {r4-r12, lr}");
		__asm__ __volatile__("mov r10, sp");
		__asm__ __volatile__("swi 123 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		__asm__ __volatile__("mov sp, r10");
		__asm__ __volatile__("ldmfd sp!, {r4-r12, lr}");*/
		break;
	case 124:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 124 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");

		/*	__asm__ __volatile__("stmfd sp!, {r4-r12, lr}");
		__asm__ __volatile__("mov r10, sp");
		__asm__ __volatile__("swi 124 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		__asm__ __volatile__("mov sp, r10");
		__asm__ __volatile__("ldmfd sp!, {r4-r12, lr}");	*/

		/*__asm__ __volatile__("mov r9, sp");
		__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 124 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		__asm__ __volatile__("mov sp, r9");
		__asm__ __volatile__("mov lr, r10");*/
		break;
	case 125:
		//__asm__ __volatile__("stmfd sp!, {r9, r10}");
		//__asm__ __volatile__("mov r9, sp");
		//__asm__ __volatile__("mov r10, lr");
		__asm__ __volatile__("swi 125 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		//__asm__ __volatile__("mov sp, r9");
		//__asm__ __volatile__("mov lr, r10");
		//__asm__ __volatile__("ldmfd sp!, {r9, r10}");

		break;
	case 126:
		__asm__ __volatile__("swi 126 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 127:
		__asm__ __volatile__("swi 127 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 128:
		__asm__ __volatile__("swi 128 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;


	case 129:
		__asm__ __volatile__("swi 129 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 130:
		__asm__ __volatile__("swi 130 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 131:
		__asm__ __volatile__("swi 131 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 132:
		__asm__ __volatile__("swi 132 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 133:
		__asm__ __volatile__("swi 133 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 134:
		__asm__ __volatile__("swi 134 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 135:
		__asm__ __volatile__("swi 135 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 136:
		__asm__ __volatile__("swi 136 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 137:
		__asm__ __volatile__("swi 137 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 138:
		__asm__ __volatile__("swi 138 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 139:
		__asm__ __volatile__("swi 139 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 140:
		__asm__ __volatile__("swi 140 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 141:
		__asm__ __volatile__("swi 141 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 142:
		__asm__ __volatile__("swi 142 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 143:
		__asm__ __volatile__("swi 143 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 144:
		__asm__ __volatile__("swi 144 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 145:
		__asm__ __volatile__("swi 145 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 146:
		__asm__ __volatile__("swi 146 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 147:
		__asm__ __volatile__("swi 147 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 148:
		__asm__ __volatile__("swi 148 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 149:
		__asm__ __volatile__("swi 149 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 150:
		__asm__ __volatile__("swi 150 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 151:
		__asm__ __volatile__("swi 151 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 152:
		__asm__ __volatile__("swi 152 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 153:
		__asm__ __volatile__("swi 153 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 154:
		__asm__ __volatile__("swi 154 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 155:
		__asm__ __volatile__("swi 155 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 156:
		__asm__ __volatile__("swi 156 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 157:
		__asm__ __volatile__("swi 157 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 158:
		__asm__ __volatile__("swi 158 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 159:
		__asm__ __volatile__("swi 159 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 160:
		__asm__ __volatile__("swi 160 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 161:
		__asm__ __volatile__("swi 161 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 162:
		__asm__ __volatile__("swi 162 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 163:
		__asm__ __volatile__("swi 163 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 164:
		__asm__ __volatile__("swi 164 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 165:
		__asm__ __volatile__("swi 165 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 166:
		__asm__ __volatile__("swi 166 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 167:
		__asm__ __volatile__("swi 167 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 168:
		__asm__ __volatile__("swi 168 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 169:
		__asm__ __volatile__("swi 169 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 170:
		__asm__ __volatile__("swi 170 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 171:
		__asm__ __volatile__("swi 171 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 172:
		__asm__ __volatile__("swi 172 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 173:
		__asm__ __volatile__("swi 173 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 174:
		__asm__ __volatile__("swi 174 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 175:
		__asm__ __volatile__("swi 175 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 176:
		__asm__ __volatile__("swi 176 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 177:
		__asm__ __volatile__("swi 177 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 178:
		__asm__ __volatile__("swi 178 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 179:
		__asm__ __volatile__("swi 179 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 180:
		__asm__ __volatile__("swi 180 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 181:
		__asm__ __volatile__("swi 181 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 182:
		__asm__ __volatile__("swi 182 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 183:
		__asm__ __volatile__("swi 183 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 184:
		__asm__ __volatile__("swi 184 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 185:
		__asm__ __volatile__("swi 185 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 186:
		__asm__ __volatile__("swi 186 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 187:
		__asm__ __volatile__("swi 187 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 188:
		__asm__ __volatile__("swi 188 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 189:
		__asm__ __volatile__("swi 189 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 190:
		__asm__ __volatile__("swi 190 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 191:
		__asm__ __volatile__("swi 191 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 192:
		__asm__ __volatile__("swi 192 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 193:
		__asm__ __volatile__("swi 193 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 194:
		__asm__ __volatile__("swi 194 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 195:
		__asm__ __volatile__("swi 195 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 196:
		__asm__ __volatile__("swi 196 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 197:
		__asm__ __volatile__("swi 197 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 198:
		__asm__ __volatile__("swi 198 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 199:
		__asm__ __volatile__("swi 199 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 200:
		__asm__ __volatile__("swi 200 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 201:
		__asm__ __volatile__("swi 201 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 202:
		__asm__ __volatile__("swi 202 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 203:
		__asm__ __volatile__("swi 203 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 204:
		__asm__ __volatile__("swi 204 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 205:
		__asm__ __volatile__("swi 205 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 206:
		__asm__ __volatile__("swi 206 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 207:
		__asm__ __volatile__("swi 207 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 208:
		__asm__ __volatile__("swi 208 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 209:
		__asm__ __volatile__("swi 209 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 210:
		__asm__ __volatile__("swi 210 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 211:
		__asm__ __volatile__("swi 211 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 212:
		__asm__ __volatile__("swi 212 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 213:
		__asm__ __volatile__("swi 213 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 214:
		__asm__ __volatile__("swi 214 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 215:
		__asm__ __volatile__("swi 215 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 216:
		__asm__ __volatile__("swi 216 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 217:
		__asm__ __volatile__("swi 217 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 218:
		__asm__ __volatile__("swi 218 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 219:
		__asm__ __volatile__("swi 219 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 220:
		__asm__ __volatile__("swi 220 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 221:
		__asm__ __volatile__("swi 221 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 222:
		__asm__ __volatile__("swi 222 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 223:
		__asm__ __volatile__("swi 223 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 224:
		__asm__ __volatile__("swi 224 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 225:
		__asm__ __volatile__("swi 225 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 226:
		__asm__ __volatile__("swi 226 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 227:
		__asm__ __volatile__("swi 227 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 228:
		__asm__ __volatile__("swi 228 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 229:
		__asm__ __volatile__("swi 229 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 230:
		__asm__ __volatile__("swi 230 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 231:
		__asm__ __volatile__("swi 231 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 232:
		__asm__ __volatile__("swi 232 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 233:
		__asm__ __volatile__("swi 233 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 234:
		__asm__ __volatile__("swi 234 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 235:
		__asm__ __volatile__("swi 235 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 236:
		__asm__ __volatile__("swi 236 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 237:
		__asm__ __volatile__("swi 237 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 238:
		__asm__ __volatile__("swi 238 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 239:
		__asm__ __volatile__("swi 239 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 240:
		__asm__ __volatile__("swi 240 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 241:
		__asm__ __volatile__("swi 241 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 242:
		__asm__ __volatile__("swi 242 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 243:
		__asm__ __volatile__("swi 243 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 244:
		__asm__ __volatile__("swi 244 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 245:
		__asm__ __volatile__("swi 245 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 246:
		__asm__ __volatile__("swi 246 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 247:
		__asm__ __volatile__("swi 247 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 248:
		__asm__ __volatile__("swi 248 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 249:
		__asm__ __volatile__("swi 249 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 250:
		__asm__ __volatile__("swi 250 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 251:
		__asm__ __volatile__("swi 251 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 252:
		__asm__ __volatile__("swi 252 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 253:
		__asm__ __volatile__("swi 253 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 254:
		__asm__ __volatile__("swi 254 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 255:
		__asm__ __volatile__("swi 255 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;
	case 256:
		__asm__ __volatile__("swi 256 " : "=r"(ret) :"r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) );
		break;

	default:
		ret = -1;
	}
	//char szSyscall[1024] = {0};
	//sprintf(szSyscall, "swi %d", n);
	//__asm__ __volatile__(szSyscall : "=r"(ret) : "r" (n));
    //	__asm__ __volatile__("swi	%0" : "=r"(ret) : "r" (n));	//	swi instruct can only be used with immediate value

    return ret;
}

#endif
