
target("Ludo.App", function()
    set_kind("binary")
    set_group("Samples/Ludo")

    local ludoProjectdir = path.join(os.projectdir(), "Samples/Ludo")
    local runtimeEnginedir = path.join(os.projectdir(), "Runtime")

    set_runargs("--projectdir", ludoProjectdir, "--enginedir", runtimeEnginedir)

    add_deps("Ludo", "Waterlily.Core", "Waterlily.Launcher", "Waterlily.Engine")

    add_files("Source/Main.cpp")

end)

-- 244168662702
