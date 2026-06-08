
local AssetsModule = BuildTool.DefaultTargetTemplate()

AssetsModule.Name = "Waterlily.Assets"
AssetsModule.Kind = "shared"
AssetsModule.Group = "Engine"

AssetsModule.Deps = {
    "Waterlily.Core"
}

AssetsModule.PrivateDefines = {
    "WL_ASSETS_EXPORTS"
}

BuildTool.RegisterTargets(AssetsModule)
