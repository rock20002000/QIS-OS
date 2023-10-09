include host_settings.mk

# Test project for kirk-os
ARCH ?= x86_64
IS_32BIT ?= n
MACHINE ?= generic

OUTDIR = temp_dir

KIRK_OUTPUT_IMAGE = $(OUTDIR)/kirk.kef

EXTRABOOT_PATH 	= bsp/build/extraboot.bin
BSP_PATH		= bsp/build/bsp.elf
KERNEL_PATH		= kernel/build/kernel.elf

APP1_PATH		= userspace/build/kirk/$(ARCH)/debug/proc.elf
APP2_PATH		= userspace/build/kirk/$(ARCH)/debug/procHello.elf
#APP3_PATH		= userspace/build/kirk/$(ARCH)/debug/procTest.elf
APP3_PATH		= userspace/build/kirk/$(ARCH)/debug/shm_owner.elf
APP4_PATH		= userspace/build/kirk/$(ARCH)/debug/testKbdDriver.elf
APP5_PATH		= userspace/build/kirk/$(ARCH)/debug/testKbdApp.elf
APP6_PATH		= userspace/build/kirk/$(ARCH)/debug/testSerialDriver.elf
APP7_PATH		= userspace/build/kirk/$(ARCH)/debug/testSerialApp.elf
APP8_PATH		= userspace/build/kirk/$(ARCH)/debug/speed.elf
APP9_PATH		= userspace/build/kirk/$(ARCH)/debug/slicing.elf
APP10_PATH		= userspace/build/kirk/$(ARCH)/debug/pulse.elf
APP11_PATH		= userspace/build/kirk/$(ARCH)/debug/mutex_alloc.elf
APP12_PATH		= userspace/build/kirk/$(ARCH)/debug/mutex.elf
APP13_PATH		= userspace/build/kirk/$(ARCH)/debug/devnow.elf
APP14_PATH		= userspace/build/kirk/$(ARCH)/debug/cat.elf
APP15_PATH		= userspace/build/kirk/$(ARCH)/debug/ls.elf
APP16_PATH		= userspace/build/kirk/$(ARCH)/debug/msg_server.elf
APP17_PATH		= userspace/build/kirk/$(ARCH)/debug/msg_client.elf
APP18_PATH		= userspace/build/kirk/$(ARCH)/debug/msgdeliver_svr.elf
APP19_PATH		= userspace/build/kirk/$(ARCH)/debug/msgdeliver_clt.elf
APP20_PATH		= userspace/build/kirk/$(ARCH)/debug/msgAttach_svr.elf
APP21_PATH		= userspace/build/kirk/$(ARCH)/debug/msgAttach_clt.elf
APP22_PATH		= userspace/build/kirk/$(ARCH)/debug/pulseAttach.elf
APP23_PATH		= userspace/build/kirk/$(ARCH)/debug/intrAttachEvt.elf
APP24_PATH		= userspace/build/kirk/$(ARCH)/debug/cache.elf
APP25_PATH		= userspace/build/kirk/$(ARCH)/debug/calls.elf
APP26_PATH		= userspace/build/kirk/$(ARCH)/debug/channel.elf
APP27_PATH		= userspace/build/kirk/$(ARCH)/debug/devnull_ctrl.elf
APP28_PATH		= userspace/build/kirk/$(ARCH)/debug/devnull.elf
APP29_PATH		= userspace/build/kirk/$(ARCH)/debug/devnullr.elf
APP30_PATH		= userspace/build/kirk/$(ARCH)/debug/malloc.elf
APP31_PATH		= userspace/build/kirk/$(ARCH)/debug/memperf.elf
APP32_PATH		= userspace/build/kirk/$(ARCH)/debug/mmap.elf
APP33_PATH		= userspace/build/kirk/$(ARCH)/debug/msg.elf
APP34_PATH		= userspace/build/kirk/$(ARCH)/debug/opens.elf
APP35_PATH		= userspace/build/kirk/$(ARCH)/debug/argc_argv.elf
APP36_PATH		= userspace/build/kirk/$(ARCH)/debug/create_process.elf
APP37_PATH		= userspace/build/kirk/$(ARCH)/debug/resmgr_process.elf
APP38_PATH		= userspace/build/kirk/$(ARCH)/debug/iotest.elf
APP39_PATH		= userspace/build/kirk/$(ARCH)/debug/ps.elf
APP40_PATH		= userspace/build/kirk/$(ARCH)/debug/mapio.elf
APP41_PATH		= userspace/build/kirk/$(ARCH)/debug/thread.elf
APP42_PATH		= userspace/build/kirk/$(ARCH)/debug/yield.elf
APP43_PATH		= userspace/build/kirk/$(ARCH)/debug/yieldchild.elf
APP44_PATH		= userspace/build/kirk/$(ARCH)/debug/latency.elf
APP45_PATH		= userspace/build/kirk/$(ARCH)/debug/timer.elf
APP46_PATH		= userspace/build/kirk/$(ARCH)/debug/testEpitDriver.elf
APP47_PATH		= userspace/build/kirk/$(ARCH)/debug/time.elf
APP48_PATH		= userspace/build/kirk/$(ARCH)/debug/vdso_test.elf
APP49_PATH		= userspace/build/kirk/$(ARCH)/debug/shm_user.elf

UBOOT_PATH		= tools/u-boot
MKIMG_PATH		= tools/mkimg

APP_PROGRAMS	=	$(APP1_PATH),\
					$(APP2_PATH),$(APP3_PATH),$(APP4_PATH),$(APP5_PATH),$(APP6_PATH),$(APP7_PATH),$(APP8_PATH),$(APP9_PATH),$(APP10_PATH),\
					$(APP11_PATH),$(APP12_PATH),$(APP13_PATH),$(APP14_PATH),$(APP15_PATH),$(APP16_PATH),$(APP17_PATH),$(APP18_PATH),$(APP19_PATH),\
					$(APP20_PATH),$(APP21_PATH),$(APP22_PATH),$(APP23_PATH),$(APP24_PATH),$(APP25_PATH),$(APP26_PATH),$(APP27_PATH),$(APP28_PATH),\
					$(APP29_PATH),$(APP30_PATH),$(APP31_PATH),$(APP32_PATH),$(APP33_PATH),$(APP34_PATH),$(APP35_PATH),$(APP36_PATH),$(APP37_PATH),\
					$(APP38_PATH),$(APP39_PATH),$(APP40_PATH),$(APP41_PATH),$(APP42_PATH),$(APP43_PATH),$(APP44_PATH),$(APP45_PATH),$(APP46_PATH),\
					$(APP47_PATH), $(APP48_PATH), $(APP49_PATH)


APP_PROGRAMS_CMD	= $(shell echo $(APP_PROGRAMS) | sed 's/ //g')

GIT_VERSION := $(shell git rev-parse --short HEAD)

ifeq ($(ARCH),arm)
MACHINE = sabrelite
IS_32BIT = y
EXBOOT_CMD = -e $(EXTRABOOT_PATH)
QEMU_OPT = qemu-system-arm -M sabrelite -smp 4 -m 1G -display none -serial null -serial mon:stdio -kernel $(UBOOT_PATH) -nic user,tftp=$(OUTDIR)

else
MACHINE = generic
EXBOOT_CMD = 
HDD_IMAGE_TPLT 	= tools/origin_bootable_hdd_kirkipl.img
HDD_IMG_OUT		= $(OUTDIR)/hdd.img
ARCH_PRE_CMDS  	= cp $(HDD_IMAGE_TPLT) $(HDD_IMG_OUT) && dd conv=notrunc bs=512 if=$(KIRK_OUTPUT_IMAGE) of=$(HDD_IMG_OUT) seek=256 status=none
qemu_drive_para := -drive id=disk,file=$(HDD_IMG_OUT),if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0
QEMU_OPT = qemu-system-x86_64 -cpu IvyBridge,rdtscp -rtc base=utc $(qemu_drive_para) -smp 1 -m 8192 -serial mon:stdio -vnc localhost:0
endif



# todo: kernel & userspace are not configured for multi-architecture yet,to be fixed
compile: clean
	mkdir -p kernel/build
	rm -vf  kernel/zCaller/*.ld
	make -C kernel/zCaller ARCH=$(ARCH) MACHINE=$(MACHINE) GIT_VERSION=$(GIT_VERSION)
	make -C sdk ARCH=$(ARCH) IS_32BIT=$(IS_32BIT)
	make -C bsp ARCH=$(ARCH) MACHINE=$(MACHINE)
	make -C kernel ARCH=$(ARCH) MACHINE=$(MACHINE) GIT_VERSION=$(GIT_VERSION)
	make -C userspace ARCH=$(ARCH) IS_32BIT=$(IS_32BIT)

build: compile
	mkdir -p $(OUTDIR)
	$(MKIMG_PATH) $(EXBOOT_CMD) -b $(BSP_PATH) -k $(KERNEL_PATH) -p $(APP_PROGRAMS_CMD) -o $(KIRK_OUTPUT_IMAGE)
	$(ARCH_PRE_CMDS)

realboard: build
	cp $(KIRK_OUTPUT_IMAGE) $(NFS_TFTP_MOUNT_DIR)/kirk.kef
	@sync
	@echo Success!
	@echo Try to run "make realboard-dbg" to connect gdb server!

realboard-dbg:
	./sabrelite_gdb.sh $(BOARD_GDB_SERVER_ADDR) $(BOARD_GDB_SERVER_PORT) $(ARM_GDB_PATH)

test:
	cp $(EXTRABOOT_PATH) $(KIRK_OUTPUT_IMAGE)
	qemu-system-arm -M sabrelite -smp 4 -m 1G -display none -serial null -serial stdio -kernel $(UBOOT_PATH) -nic user,tftp=$(OUTDIR)

run: 
	$(QEMU_OPT)

debug:
	$(QEMU_OPT) -s -S

trace:
	cgdb -d arm-none-eabi-gdb -x "cgdb.script"



clean:
	make -C bsp clean ARCH=$(ARCH) MACHINE=$(MACHINE)
	make -C kernel clean ARCH=$(ARCH) MACHINE=$(MACHINE)
	make -C userspace dist-clean
	make -C sdk dist-clean
	rm -rf $(OUTDIR)