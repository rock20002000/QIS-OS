
objs-y += 	init.o \
			arm_fault.o \
			arm_page.o \
			cpu_common.o \
			arm_sched.o \
			interrupt.o \
			bios_rw.o \
			trap.o  \
			gic.o  \
			context.o \
			context_swt.o  \
			ltimer.o   \
			vdso_text.o  \
			mpcore.o  \
			smp.o
			
subdirs-y += boot