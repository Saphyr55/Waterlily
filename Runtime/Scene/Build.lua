local SceneModule = BuildTool.DefaultTargetTemplate()

SceneModule.Name = "Waterlily.Scene"
SceneModule.Kind = "shared"
SceneModule.Group = "Engine"
SceneModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Assets",
    "Waterlily.Entity"
}

SceneModule.Defines = {
    "WL_SCENE_EXPORTS"
}

BuildTool.RegisterTargets(SceneModule)
BuildTool.RegisterModules(SceneModule)
