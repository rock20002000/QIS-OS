objs-y += 	main.o \
			read_kef.o \
			block_dev.o \
			$(ARCH)/arch_start.o \
			$(ARCH)/run_next.o


subdirs-y += $(ARCH)/$(MACHINE)