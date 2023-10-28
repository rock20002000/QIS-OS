
export ARCH ?= x86_64
BUILD_DIR ?= build
MACHINE ?= generic
CONFIG_USING_EXBOOT ?= N

# The SDK path should be configured first!!
SDK_PATH := ../sdk

LIBC_PATH := $(SDK_PATH)/build/kirk/$(ARCH)/debug/libkern.a 

ifeq ($(ARCH),x86_64)
CC = gcc
ASM = $(CC)
LD = ld
AR = ar
OBJCOPY = objcopy
OBJDUMP = objdump

#ARCH_CFLAGS := -m64 -mno-mmx -mno-sse -mno-sse2 -mno-avx -mno-avx2 -mno-red-zone -mcmodel=large
ARCH_CFLAGS := -m64 -mno-mmx -mno-sse -mno-sse2 -mno-avx -mno-avx2 -mcmodel=large
LDFLAGS := -m elf_x86_64

else ifeq ($(ARCH),arm)
CC = arm-none-eabi-gcc
ASM = $(CC)
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

ARCH_CFLAGS := -mcpu=cortex-a9
LDFLAGS := 
LIBC_PATH += $(SDK_PATH)/prebuilt/$(ARCH)/libgcc.a
else
$(error Please set a supported arch!,current arch is $(ARCH))
endif