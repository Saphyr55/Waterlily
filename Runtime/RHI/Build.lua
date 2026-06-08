
local RHIModule = BuildTool.DefaultTargetTemplate()

RHIModule.Name = "Waterlily.RHI"
RHIModule.Kind = "shared"
RHIModule.Group = "Engine" 
RHIModule.Deps = {
    "Waterlily.Core"
}

RHIModule.Defines = {
    "WL_RHI_EXPORTS"
}

BuildTool.RegisterTargets(RHIModule)
BuildTool.RegisterModules(RHIModule)
