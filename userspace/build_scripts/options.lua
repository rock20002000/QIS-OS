-- option: 32bit
option("32bit")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("The target machine type,default is 64bit machine.")
    set_configvar("KIRK_CONFIG_BIT_32", 1)
option_end()

-- option: target-arch
option("target-arch")
    set_default("x86")
    set_showmenu(true)
    set_category("arch")
    set_description("The target architecture.")
    set_values("x86", "arm", "riscv")
option_end()