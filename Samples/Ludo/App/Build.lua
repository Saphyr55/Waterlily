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

-- TODO: Check if we are in dev mode.
table.insert(LudoAppTarget.Deps, "Ludo.Dev")

LudoAppTarget.Sources = {
    "Source/Main.cpp"
}

LudoAppTarget.Callback = function()
    BuildTool.GenerateModuleManifest()
end

BuildTool.RegisterTargets(LudoAppTarget)
