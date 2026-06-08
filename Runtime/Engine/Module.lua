local EngineModule = {}

EngineModule.Name = "Waterlily.Engine"
EngineModule.Kind = "shared"
EngineModule.Group = "Engine"

EngineModule.Deps = {
    "Waterlily.Core"
}

EngineModule.PublicIncludes = {
    "Source"
}
EngineModule.Headers = {
    "Source/**.hpp"
}
EngineModule.Sources = {
    "Source/**.cpp"
}
EngineModule.PrivateDefines = {
    "WL_ENGINE_EXPORTS"
}

BuildTool.RegisterTargets(EngineModule)
