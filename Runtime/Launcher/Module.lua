local LauncherModule = BuildTool.DefaultTargetTemplate()

LauncherModule.Name = "Waterlily.Launcher"
LauncherModule.Kind = "shared"
LauncherModule.Group = "Engine"

LauncherModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine"
}

LauncherModule.PrivateDefines = {
    "WL_LAUNCHER_EXPORTS"
}

LauncherModule.Callback = function()
    if is_plat("windows") then
        add_syslinks("User32")
    end
end

BuildTool.RegisterTargets(LauncherModule)
