# filename: .gdbinit
# gdb will read it when starting

file build/kernel.elf.debug
target remote localhost:1234


