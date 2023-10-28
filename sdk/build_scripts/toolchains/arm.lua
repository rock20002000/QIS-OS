toolchain("cc_arm")
    -- set toolset
    set_kind("standalone")

    -- init flags 
    if(has_config("32bit")) then
        set_toolset("cc", "arm-none-eabi-gcc")
        set_toolset("ld", "arm-none-eabi-ld")
        set_toolset("ar", "arm-none-eabi-ar")
        set_toolset("ex", "arm-none-eabi-ar")
        set_toolset("as", "arm-none-eabi-gcc")
    else
        set_toolset("cc", "arm-none-eabi-gcc")
        set_toolset("ld", "arm-none-eabi-ld")
        set_toolset("ar", "arm-none-eabi-ar")
        set_toolset("ex", "arm-none-eabi-ar")
        set_toolset("as", "arm-none-eabi-gcc")
    end

    on_load(function (toolchain)
    	local march = (has_config("32bit") and "-mcpu=cortex-a9" or "-m64")
        -- local march = (has_config("32bit") and "-march=armv7-a" or "-m64")
        toolchain:add("cxflags", march)
        -- toolchain:add("ldflags", march)
        toolchain:add("asflags", march)

    end)

toolchain_end()