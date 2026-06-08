local EngineModule = BuildTool.DefaultTargetTemplate()

EngineModule.Name = "Waterlily.Engine"
EngineModule.Kind = "shared"
EngineModule.Group = "Engine"

EngineModule.Deps = {
    "Waterlily.Core"
}

EngineModule.Defines = {
    "WL_ENGINE_EXPORTS"
}

BuildTool.RegisterTargets(EngineModule)
BuildTool.RegisterModules(EngineModule)
