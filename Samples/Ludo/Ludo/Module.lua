local LudoModule = {}

LudoModule.Name = "Ludo"
LudoModule.Kind = "shared"
LudoModule.Group = "Samples/Ludo"
LudoModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.RHI",
    "Waterlily.RHI.Vulkan",
    "Waterlily.Messaging",
    "Waterlily.Scene",
    "Waterlily.Renderer",
    "Waterlily.Entity",
    "Waterlily.Assets"
}

LudoModule.PublicIncludes = {
    "Source"
}
LudoModule.Sources = {
    "Source/**.cpp"
}
LudoModule.Headers = {
    "Source/**.hpp"
}

LudoModule.PrivateDefines = {
    "WL_LUDO_EXPORTS"
}

BuildTool.RegisterTargets(LudoModule)