

target_dir_prefix = "../"

-- The proc target
target("proc.elf")
    set_kind("binary")
    -- Should disable in release version

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")
	add_includedirs(target_dir_prefix..board_inc_path)
	
    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../service/proc/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/proc.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/proc.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/proc.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The procHello target
target("procHello.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/procHello/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/procHello.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/procHello.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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




-- The procTest target
target("procTest.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/procTest/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/procTest.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/procTest.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/procTest.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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


-- The testKbdDriver target
target("testKbdDriver.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")
    
    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../driver/testKbdDriver/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testKbdDriver.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testKbdDriver.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The testKbdApp target
target("testKbdApp.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/testKbdApp/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true}) 
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testKbdApp.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testKbdApp.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The testSerialDriver target
target("testEpitDriver.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")
    add_includedirs(target_dir_prefix..board_inc_path)

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../driver/testEpitDriver/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testEpitDriver.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testSerialDriver.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The testSerialDriver target
target("testSerialDriver.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")
    add_includedirs(target_dir_prefix..board_inc_path)

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../driver/testSerialDriver/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testSerialDriver.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testSerialDriver.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The testSerialApp target
target("testSerialApp.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/testSerialApp/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testSerialApp.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/testSerialApp.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The vdso_test target
target("vdso_test.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/vdso_test/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/vdso_test.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/vdso_test.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The shm_owner target
target("shm_owner.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/shm_owner/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/shm_owner.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/shm_owner.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The shm_user target
target("shm_user.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/shm_user/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/shm_user.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/shm_user.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The speed target
target("speed.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/speed/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/speed.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The slicing target
target("slicing.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/slicing/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/slicing.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The pulse target
target("pulse.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/pulse/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/pulse.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The mutex_alloc target
target("mutex_alloc.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/mutex_alloc/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mutex_alloc.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The mutex target
target("mutex.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/mutex/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mutex.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The devnow target
target("devnow.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/devnow/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/devnow.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The cat target
target("cat.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../utils/cat/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/cat.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The ls target
target("ls.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../utils/ls/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/ls.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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

-- The ps target
target("ps.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../utils/ps/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/ps.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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

-- The time target
target("time.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../utils/time/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/time.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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

-- The msg_server target
target("msg_server.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msg_server/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msg_server.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The msg_client target
target("msg_client.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msg_client/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msg_client.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msg_client.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The msgdeliverevent_server target
target("msgdeliver_svr.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msgdeliverevent_server/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msgdeliver_svr.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The msgdeliverevent_client target
target("msgdeliver_clt.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msgdeliverevent_client/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msgdeliver_clt.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The msgAttach_server target
target("msgAttach_svr.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msgAttach_server/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true}) 
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msgAttach_svr.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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



-- The msgAttach_client target
target("msgAttach_clt.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msgAttach_client/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msgAttach_clt.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The pulseAttach target
target("pulseAttach.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/pulseAttach/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/pulseAttach.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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


-- The interruptAttachEvent target
target("intrAttachEvt.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")
	add_includedirs(target_dir_prefix..board_inc_path)

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/interruptAttachEvent/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/intrAttachEvt.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The cache target
target("cache.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/cache/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/cache.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})



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


-- The calls target
target("calls.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/calls/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/calls.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The channel target
target("channel.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/channel/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true}) 
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/channel.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The devnull_control target
target("devnull_ctrl.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/devnull_control/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/devnull_ctrl.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The devnull target
target("devnull.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/devnull/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/devnull.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/devnull.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The devnullr target
target("devnullr.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/devnullr/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/devnullr.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The malloc target
target("malloc.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/malloc/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true}) 
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/malloc.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The memperf target
target("memperf.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/memperf/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/memperf.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The mmap target
target("mmap.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/mmap/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mmap.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The msg target
target("msg.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/msg/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/msg.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The opens target
target("opens.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/opens/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/opens.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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





-- The argc_argv target
target("argc_argv.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/argc_argv/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/argc_argv.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The create_process target
target("create_process.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/create_process/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/create_process.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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



-- The resmgr_process target
target("resmgr_process.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/resmgr_process/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/resmgr_process.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The iotest target
target("iotest.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/iotest/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
        add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end
	
	add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/iotest.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
 
    -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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


-- The mapio target
target("mapio.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../application/mapio/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mapio.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mapio.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/mapio.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The timer target
target("timer.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/timer/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})

    if is_arch("x86_64") then
    add_ldflags("-m elf_x86_64", {force = true})
    elseif is_arch("arm") then
    add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
    end

    add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/iotest.map","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
     -- add link flag
    -- add_ldflags("-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})

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

-- The thread target
target("thread.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/thread/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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




-- The yield target
target("yield.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/yield/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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




-- The yieldchild target
target("yieldchild.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/yieldchild/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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



-- The latency target
target("latency.elf")
    set_kind("binary")

    add_options("32bit")
    add_options("target-arch")

    -- Add include directory for kirk apps
    add_includedirs(target_dir_prefix..sdk_path.."/includes", target_dir_prefix..sdk_path.."/lib")
    add_includedirs(target_dir_prefix..sdk_path.."/lib/platform/arch/$(arch)/include")

    -- add kirk common start file in sdk
    -- (already linked in libplatform.a,just link it together)

    -- add_files of this program
    add_files("../benchmark/latency/*.c")

    -- add sdk libs
    add_ldflags(lib_path..lib_name, {force = true})
    add_ldflags(lib_path..lib_utility, {force = true})

    if is_arch("x86_64") then
        -- add_ldflags("-m elf_x86_64", {force = true})
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    elseif is_arch("arm") then
        add_ldflags("../sdk/prebuilt/arm/libgcc.a", {force = true})
    -- add_ldflags("-m armelf");
        add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m armelf","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})
    end

    -- add link flag
    -- add_ldflags("-Map $(buildir)/$(plat)/$(arch)/$(mode)/thread.map","-m elf_x86_64","-T "..linkscript, "-nostdlib -static --gc-sections",{force = true})


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