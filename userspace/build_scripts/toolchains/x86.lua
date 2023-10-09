-- https://xmake.io/#/zh-cn/manual/custom_toolchain?id=toolchain
-- https://xmake.io/#/zh-cn/manual/project_target?id=targetset_toolset

toolchain("cc_x86")
    -- set toolset
    set_kind("standalone")
    set_toolset("cc", "gcc")
    set_toolset("ld", "ld")
    set_toolset("ar", "ar")
    set_toolset("ex", "ar")
    set_toolset("as", "gcc")

    on_load(function (toolchain)

        -- init flags 
        local march = (has_config("32bit") and "-m32" or "-m64")
        toolchain:add("cxflags", march)
        -- toolchain:add("ldflags", march)
        toolchain:add("asflags", march)

        if(has_config("32bit")) then

        else
            toolchain:add("cxflags","-D_KIRK_CONFIG_MACHINE_64BIT_")
        end
    end)

toolchain_end()