
include xconfig.mk

EXTRA_INCLUDE_PATH := -I../../sdk/lib -I../../sdk/includes  -I../../sdk/lib/libc/include

LINKER_FILE := vdso.ld

fobj = ../build
vobjs-y := vdso.o
vobjs := $(foreach F,$(vobjs-y),$(fobj)/$F)

CUR_C:=${wildcard *.c}
C_OBJ:=${patsubst %.c, %.o, $(CUR_C)}
CUR_ASM_SOURCE:=${wildcard *.S}
ASM_OBJ:=${patsubst %.S, %.o, $(CUR_ASM_SOURCE)}

VDSO_LDFLAGS = -shared $(call ld-option, --eh-frame-hdr) -Bsymbolic -nostdlib -Tvdso.ld $(GCC_LIB)
all: $(LINKER_FILE)  $(C_OBJ)  $(ASM_OBJ)  
$(C_OBJ):%.o: %.c
	$(CC) $(CC_FLAG) $(CFL)  $(EXTRA_INCLUDE_PATH) -o $(fobj)/$@ -c $<
	$(CC) $(CC_FLAG) $(VDSO_LDFLAGS) $(EXTRA_INCLUDE_PATH) -o $(fobj)/vdso.so $(fobj)/$@
	$(OBJDUMP) -t -S $(fobj)/vdso.so > $(fobj)/vdso.sym

$(ASM_OBJ):%.o: %.S
	$(CC) $(ASM_PARAM) -D__ASSEMBLY__   $(EXTRA_INCLUDE_PATH) -o $(fobj)/$@ -c $<

# Example for hex output by shell: echo $(printf 0x%x $result)
$(LINKER_FILE):vdso.tplt
#	@mkdir -p $(@D)
	cp vdso.tplt vdso.ld
	@echo $CONFIG_VDSO_FORMAT
	sed -i "s/MARK_VDSO_FORMAT/$(CONFIG_VDSO_FORMAT)/g" vdso.ld
	sed -i "s/MARK_VDSO_ARCH/$(CONFIG_VDSO_ARCH)/g" vdso.ld