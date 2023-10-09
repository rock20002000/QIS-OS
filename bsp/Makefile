
include config/config.mk
include config/$(ARCH)/$(MACHINE)/xconfig.mk


INCLUDES := -I$(SDK_PATH)/includes -I$(SDK_PATH)/lib -I$(SDK_PATH)/lib/boot/include/arch/$(ARCH)
INCLUDES += -Isource/hardware/startup -Isource/hardware/startup/machine/arch/$(ARCH) -Isource/hardware/startup/lib/arch/$(ARCH)
INCLUDES += -Isource/hardware/startup/machine/arch/$(ARCH)/$(MACHINE) -Isource/hardware/extra-boot/$(ARCH)/$(MACHINE)
INCLUDES += -I../include_board
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


bsp_image := $(BUILD_DIR)/bsp.elf

ifeq ($(CONFIG_USING_EXBOOT),Y)
extraboot_image := $(BUILD_DIR)/extraboot.bin
else
extraboot_image :=
endif



all: $(extraboot_image) $(bsp_image)


# Get objects of bsp
objs :=
$(eval $(call visit-subdir,source/hardware/startup,$(BUILD_DIR)))
bsp_objs := $(objs)
bsp_objs += $(LIBC_PATH)


CC_FLAGS_COMMON = -g -Og -fno-pic -ffreestanding -nostdinc -nostdlib -fno-builtin -fno-exceptions -Wall -Wno-comment 
CC_FLAGS_COMMON += -fno-omit-frame-pointer -fno-common
CC_FLAGS_COMMON += -ffunction-sections -fdata-sections
#
#  BSP Build Rules
#

CC_FLAG_BSP = $(CC_FLAGS_COMMON) 

CC_FLAG_BSP += $(ARCH_CFLAGS)

CC_FLAG_BSP += -DUSING_DEBUG

LINKER_FILE := $(BUILD_DIR)/generated_files/bsp-linker.ld # source/hardware/startup/lib/arch/$(ARCH)/linker.ld

$(bsp_image): $(bsp_objs) $(LINKER_FILE)
	$(LD) $(LDFLAGS) -T $(LINKER_FILE) -Map $(@:.elf=.map) -o $@.debug $(bsp_objs)
	$(OBJCOPY) -S $@.debug $@

$(LINKER_FILE): source/hardware/startup/lib/arch/$(ARCH)/bsp_linker.tplt
	mkdir -p $(@D)
	cp $< $@
	sed -i "s/CONFIG_RAM_START/$(CONFIG_RAM_START)/g" $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CC_FLAG_BSP) $(INCLUDES) -c -o $@ $< 

	
$(BUILD_DIR)/%.o: %.S
	mkdir -p $(@D)
	$(ASM) $(CFLAGS) $(CC_FLAG_BSP) $(INCLUDES) -c -o $@ $< 

#
#  Extra boot Build Rules
#

CC_FLAG_EXTRABOOT = $(CC_FLAGS_COMMON) 

CC_FLAG_EXTRABOOT += $(ARCH_CFLAGS)

$(BUILD_DIR)/source/hardware/extra-boot/%.o: source/hardware/extra-boot/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CC_FLAG_EXTRABOOT) $(INCLUDES) -c -o $@ $< 

$(BUILD_DIR)source/hardware/extra-boot/%.o: source/hardware/extra-boot/%.S
	mkdir -p $(@D)
	$(ASM) $(CFLAGS) $(CC_FLAG_EXTRABOOT) $(INCLUDES) -c -o $@ $< 

# Get objects
objs :=
$(eval $(call visit-subdir,source/hardware/extra-boot,$(BUILD_DIR)))
extraboot_objs := $(objs)
extraboot_objs += $(LIBC_PATH)

EB_LINKER_FILE := $(BUILD_DIR)/generated_files/boot-linker.ld
EB_BOARD_CONFIG_FILE := source/hardware/extra-boot/$(ARCH)/$(MACHINE)/board_cfg.h

$(extraboot_image): $(EB_LINKER_FILE) $(EB_BOARD_CONFIG_FILE) $(extraboot_objs)
	$(LD) $(LDFLAGS) -T $(EB_LINKER_FILE) -Map $(@:.bin=.map) -o $(@:.bin=.elf) $(extraboot_objs)
	$(OBJCOPY) -O binary --pad-to=$(CONFIG_EXBOOT_END_ADDR) --gap-fill=0x0 $(@:.bin=.elf) $@


# Example for hex output by shell: echo $(printf 0x%x $result)
$(EB_LINKER_FILE): source/hardware/extra-boot/template/extraboot_linker.tplt
	@mkdir -p $(@D)
	@cp $< $@
	@sed -i "s/CONFIG_RAM_START/$(CONFIG_EXBOOT_ADDR)/g" $@
	@sed -i "s/CONFIG_RAM_END_ADDR/$(CONFIG_EXBOOT_END_ADDR)/g" $@

$(EB_BOARD_CONFIG_FILE): source/hardware/extra-boot/template/extraboot_cfg_tplt.h
	@mkdir -p $(@D)
	@cp $< $@
	@sed -i "s/CONFIG_IO_START/$(CONFIG_IO_START)/g" $@
	@sed -i "s/CONFIG_IO_END/$(CONFIG_IO_END)/g" $@
	@sed -i "s/CONFIG_RAM_START/$(CONFIG_RAM_START)/g" $@
	@sed -i "s/CONFIG_RAM_END/$(CONFIG_RAM_END)/g" $@
	@sed -i "s/CONFIG_EXBOOT_START/$(CONFIG_EXBOOT_ADDR)/g" $@
	@sed -i "s/CONFIG_EXBOOT_END/$(CONFIG_EXBOOT_END_ADDR)/g" $@
	@sed -i "s/CONFIG_KIRK_LOAD_ALIGN/$(CONFIG_KIRK_LOAD_ALIGN)/g" $@
	@sed -i "s/CONFIG_PPTR_TOP/$(CONFIG_PPTR_TOP)/g" $@
	@sed -i "s/CONFIG_BOOTUP_ELF_PADDR_BASE/$(CONFIG_BOOTUP_ELF_PADDR_BASE)/g" $@
	@sed -i "s/CONFIG_KERNEL_ELF_PADDR_BASE/$(CONFIG_KERNEL_ELF_PADDR_BASE)/g" $@

extra-boot: $(extraboot_image)
	@echo $^ build success!

clean:
	rm -rf $(BUILD_DIR)

.PHONY: $(EB_BOARD_CONFIG_FILE) $(EB_LINKER_FILE)