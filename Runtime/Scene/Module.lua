local SceneModule = {}

SceneModule.Name = "Waterlily.Scene"
SceneModule.Kind = "shared"
SceneModule.Group = "Engine"
SceneModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Assets",
    "Waterlily.Entity"
}
SceneModule.PublicIncludes = {
    "Source"
}
SceneModule.Sources = {
    "Source/**.cpp"
}
SceneModule.Headers = {
    "Source/**.hpp"
}
SceneModule.PrivateDefines = {
    "WL_SCENE_EXPORTS"
}

BuildTool.RegisterTargets(SceneModule)
