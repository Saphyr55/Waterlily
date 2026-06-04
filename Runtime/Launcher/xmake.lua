
target("Waterlily.Launcher", function()
    set_kind("shared")
    set_group("Engine")

    add_deps("Waterlily.Core", "Waterlily.Engine")

    target_files_default({
        public = true
    })

    add_defines("WL_LAUNCHER_EXPORTS")

    if is_plat("windows") then
        add_syslinks("User32")
    end
end)

