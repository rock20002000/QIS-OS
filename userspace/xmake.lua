-- configure the sdk settings
sdk_path = "../sdk"
board_inc_path = "../include_board"

lib_path = sdk_path.."/build/kirk/$(arch)/debug/"
lib_name = "libplatform.a"
lib_utility = "libutility.a"

linkscript = sdk_path.."/lib/platform/startup/arch/$(arch)/kirk.ld"

-- set supported target architecture
set_allowedplats("kirk")

set_allowedarchs("arm","x86_64","riscv")


set_defaultplat("kirk")
set_defaultarchs("x86_64")
set_defaultmode("debug")

-- project
set_project("kirk-programs")

-- set xmake minimum version
set_xmakever("2.5.1")

-- set warning all as error
-- set_warnings("all", "error")

-- set language: gnu99
stdc = "gnu99"
set_languages(stdc)


-- set some common compile flags
cflgs = "-fno-pic -ffreestanding -nostdinc -nostdlib -fno-builtin -fno-exceptions -Wall -fno-omit-frame-pointer -fno-common -ffunction-sections -fdata-sections"
--	cflgs = "-fno-pic -ffreestanding -mno-red-zone -nostdinc -nostdlib -fno-builtin -fno-exceptions -Wall -fno-omit-frame-pointer -fno-common -ffunction-sections -fdata-sections"

add_cxflags(cflgs)
add_asflags(cflgs)

includes("build_scripts/toolchains/*.lua")
includes("build_scripts/*.lua")