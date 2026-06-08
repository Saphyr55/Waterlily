local LudoAppTarget = {}
LudoAppTarget.Name = "Ludo.App"
LudoAppTarget.Kind = "binary"
LudoAppTarget.Group = "Samples/Ludo"

LudoAppTarget.Deps = {
    "Ludo",
    "Waterlily.Core",
    "Waterlily.Launcher",
    "Waterlily.Engine"
}

LudoAppTarget.Sources = {
    "Source/Main.cpp"
}

LudoAppTarget.Callback = function()
    local ludoProjectdir = path.join(os.projectdir(), "Samples/Ludo")
    local runtimeEnginedir = path.join(os.projectdir(), "Runtime")

    set_runargs("--projectdir", ludoProjectdir, "--enginedir", runtimeEnginedir)
end

BuildTool.RegisterTargets(LudoAppTarget)
