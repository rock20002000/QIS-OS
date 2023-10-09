include config.mk


BUILD_DIR ?= build

kernel_image := $(BUILD_DIR)/kernel.elf
kernel_ld := kernel/arch/$(ARCH)/kernel.ld

MAJOR_VERSION := 1
MINOR_VERSION := 0
BUILD_NO      := 0

CC_FLAGS_COMMON = -g -Og -fno-pic -ffreestanding -nostdinc -nostdlib -fno-builtin -fno-exceptions -Wall -Wno-comment 
CC_FLAGS_COMMON += -fno-omit-frame-pointer -fno-common
CC_FLAGS_COMMON += -ffunction-sections -fdata-sections

CC_FLAG_KERN += $(DEBUG_MODE) -DKERNEL $(ARCH_CFLAGS) $(CC_FLAGS_COMMON) -DGIT_VERSION=$(GIT_VERSION) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -DBUILD_NO=$(BUILD_NO)

LDFLAGS := -nostdlib -static
LDFLAGS += --gc-sections -T $(kernel_ld) 
# -Map $(@:.elf=.map)

all: $(kernel_image)




# Visits the soruce directory recursively and fills $(cflags), $(objs) and $(libs).
# $(1): The target source dir.
# $(2): The build dir.
define visit-subdir
$(eval objs-y :=)
$(eval libs-y :=)
$(eval build_dir := $(2)/$(1))
$(eval subdirs-y :=)
$(eval cflags-y :=)
$(eval global-cflags-y :=)
$(eval global-includes-y :=)
$(eval include $(1)/build.mk)
$(eval build_mks += $(1)/build.mk)
$(eval objs     += $(addprefix $(2)/$(1)/, $(objs-y)))
$(eval all_objs += $(addprefix $(2)/$(1)/, $(objs-y)))
$(eval libs += $(libs-y))
$(eval cflags += $(cflags-y))
$(eval CFLAGS += $(global-cflags-y))
$(eval INCLUDES += $(global-includes-y))
$(eval $(foreach subdir, $(subdirs-y), \
	$(eval $(call visit-subdir,$(1)/$(subdir),$(2)))))
endef

# Enumerate kernel object files.
all_objs :=
objs :=
$(eval $(call visit-subdir,kernel,$(BUILD_DIR)))
kernel_objs := $(objs)
kernel_objs += $(LIBC_PATH)


INCLUDES += -Ikernel/include -Ikernel
INCLUDES += -Ikernel/arch/$(ARCH)/include -Ikernel/arch/$(ARCH)

INCLUDES += -I$(SDK_PATH)/includes -I$(SDK_PATH)/lib
INCLUDES += -I$(SDK_PATH)/lib/utils/include 
INCLUDES += -I$(SDK_PATH)/lib/boot/include/arch/$(ARCH)
INCLUDES += -I$(SDK_PATH)/lib/platform/arch/$(ARCH)/include

#
#  Kernel Build Rules
#

$(kernel_image): $(kernel_objs) $(kernel_ld)
	$(LD) $(ARCH_LDFLAGS) $(LDFLAGS) -Map $(@:.elf=.map) -o $@.debug $(kernel_objs)
	$(OBJCOPY) -S $@.debug $@
	$(OBJDUMP) -t -S $@.debug > $(BUILD_DIR)/kernel.sym


$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CC_FLAG_KERN) $(INCLUDES)  -c -o $@ $< 

$(BUILD_DIR)/%.o: %.S
	mkdir -p $(@D)
	$(ASM) $(CFLAGS) $(CC_FLAG_KERN) $(INCLUDES) -c -o $@ $< 




clean:
	rm -rf build

