-- set supported target architecture
set_allowedplats("kirk")

set_allowedarchs("arm","x86_64","riscv")


set_defaultplat("kirk")
set_defaultarchs("x86_64")
set_defaultmode("debug")

-- project
set_project("kirk_libs")

-- set xmake minimum version
set_xmakever("2.5.1")

-- set warning all as error
-- set_warnings("all", "error")

-- set language: gnu99
stdc = "gnu99"
set_languages(stdc)


-- set some common compile flags
cflgs = "-fno-pic -ffreestanding -nostdinc -nostdlib -fno-builtin -fno-exceptions -Wall -fno-omit-frame-pointer -fno-common -ffunction-sections -fdata-sections"

add_cxflags(cflgs,{force = true})
add_asflags(cflgs,{force = true})


add_includedirs("includes")
add_includedirs("lib")

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")
add_asflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")


includes("build_scripts/*.lua")
includes("build_scripts/toolchains/*.lua")

