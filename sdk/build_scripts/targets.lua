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

target("kern")
    set_kind("static")

    add_options("32bit")
    add_options("target-arch")

    add_includedirs("../lib/platform/arch/$(arch)/include")
    
    -- add_includedirs("../lib/utils/include")
    -- add_includedirs("../lib/boot/include")
    -- add_files("libc/*.c")
    add_files("../lib/libc/*/*.c")
    add_files("../lib/utils/*.c")

	add_files("../lib/platform/arch/$(arch)/vga_rw.c")

    -- if is_arch("x86_64") then
    -- add_files("../lib/platform/arch/x86_64/vga_rw.c")
    -- end


    -- if is_config("target-arch","x86") then
    --     set_toolchains("cc_x86")      
    -- elseif is_config("target-arch","arm") then
    --     set_toolchains("cc_arm")
    -- end

    if is_arch("x86_64") then
        set_toolchains("cc_x86")
        add_cxflags("-mcmodel=large")
        add_asflags("-mcmodel=large")    
    elseif is_arch("arm") then
        set_toolchains("cc_arm")
    end

    if is_mode("release") then
        add_cxflags("-O0") 
        add_asflags("-O0") 
    else 
        add_cxflags("-g","-O0")    
        add_asflags("-g","-O0") 
    end
    add_cxflags("-DKERNEL") 
target_end()

target("platform")
    set_kind("static")

    add_options("32bit")
    add_options("target-arch")


    add_includedirs("../lib/platform/arch/$(arch)/include")

    -- add_files("libc/*.c")
    add_files("../lib/libc/*/*.c")
    add_files("../lib/platform/*/*.c")
    add_files("../lib/libc/atomic/arch/$(arch)/*.S")    
    add_files("../lib/platform/startup/arch/$(arch)/*.S")
	-- add_files("../lib/platform/arch/$(arch)/inout.c")
	add_files("../lib/platform/arch/$(arch)/rand.c")

    if is_arch("x86_64") then
        set_toolchains("cc_x86")      
    elseif is_arch("arm") then
        set_toolchains("cc_arm")
    end

    if is_mode("release") then
        add_cxflags("-O0") 
        add_asflags("-O0") 
    else 
        add_cxflags("-g","-O0")    
        add_asflags("-g","-O0") 
    end
target_end()

target("utility")
    set_kind("static")

    add_options("32bit")
    add_options("target-arch")

    -- add_files("libc/*.c")
    add_files("../lib/utils/*.c")


    -- if is_config("target-arch","x86") then
    --     set_toolchains("cc_x86")      
    -- elseif is_config("target-arch","arm") then
    --     set_toolchains("cc_arm")
    -- end

    if is_arch("x86_64") then
        set_toolchains("cc_x86")
        add_cxflags("-mcmodel=large")
        add_asflags("-mcmodel=large")    
    elseif is_arch("arm") then
        set_toolchains("cc_arm")
    end

    if is_mode("release") then
        add_cxflags("-O0") 
        add_asflags("-O0") 
    else 
        add_cxflags("-g","-O0")    
        add_asflags("-g","-O0") 
    end
target_end()