
local RHIModule = {}
RHIModule.Name = "Waterlily.RHI"
RHIModule.Kind = "shared"
RHIModule.Group = "Engine" 
RHIModule.Deps = {
    "Waterlily.Core"
}

RHIModule.PublicIncludes = {
    "Source"
}
RHIModule.Headers = {
    "Source/**.hpp"
}
RHIModule.Sources = {
    "Source/**.cpp"
}
RHIModule.PrivateDefines = {
    "WL_RHI_EXPORTS"
}

BuildTool.RegisterTargets(RHIModule)