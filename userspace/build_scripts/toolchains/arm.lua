toolchain("cc_arm")
    -- set toolset
    set_kind("standalone")

    -- init flags 
    if(has_config("32bit")) then
        set_toolset("cc", "arm-none-eabi-gcc -DARM")
        set_toolset("ld", "arm-none-eabi-ld")
        set_toolset("ar", "arm-none-eabi-ar")
        set_toolset("ex", "arm-none-eabi-ar")
        set_toolset("as", "arm-none-eabi-gcc")
    else
        set_toolset("cc", "aarch64-none-elf-gcc -DARM")
        set_toolset("ld", "aarch64-none-elf-ld")
        set_toolset("ar", "aarch64-none-elf-ar")
        set_toolset("ex", "aarch64-none-elf-ar")
        set_toolset("as", "aarch64-none-elf-gcc")
    end

    on_load(function (toolchain)
        local march = (has_config("32bit") and "-mcpu=cortex-a9" or "-march=armv8-a")
        -- local march = (has_config("32bit") and "-march=armv7-a" or "-march=armv8-a")
        toolchain:add("cxflags", march)
        -- toolchain:add("ldflags", march)
        toolchain:add("asflags", march)

    end)

toolchain_end()