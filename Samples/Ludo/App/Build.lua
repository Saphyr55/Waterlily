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
    BuildTool.GenerateModuleManifest()
end

BuildTool.RegisterTargets(LudoAppTarget)
