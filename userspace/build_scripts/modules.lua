-- Services
-- ****************************************************** --

-- option: proc
option("service_proc")
    set_default(true)
    set_showmenu(true)
    set_category("Modules/service")
    set_description("[PROC] The proc process,1st userspace program in kirkos")
option_end()


-- Drivers
-- ****************************************************** --

-- option: drv_8250
option("drv_8250")
    set_default(false)
    set_showmenu(true)
    set_category("Modules/drivers")
    set_description("[SERIAL] The 8250 serial controller driver")
option_end()


-- Applications
-- ****************************************************** --

-- option: app_hello_world
option("app_hello_world")
    set_default(false)
    set_showmenu(true)
    set_category("Modules/App")
    set_description("[Hello] Hello world program")
option_end()

-- option: app_benchmark
option("app_benchmark")
    set_default(false)
    set_showmenu(true)
    set_category("Modules/App")
    set_description("[Benchmark] The benchmark routine")
option_end()