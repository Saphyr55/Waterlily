local LudoModule = BuildTool.DefaultTargetTemplate()

LudoModule.Name = "Ludo"
LudoModule.Kind = "shared"
LudoModule.Group = "Samples/Ludo"
LudoModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.RHI",
    "Waterlily.Messaging",
    "Waterlily.Scene",
    "Waterlily.Renderer",
    "Waterlily.Entity",
    "Waterlily.Assets"
}

-- TODO: Check if we enabled vulkan.
table.insert(LudoModule.Deps, "Waterlily.RHI.Vulkan")

LudoModule.Defines = {
    "WL_LUDO_EXPORTS"
}

BuildTool.RegisterTargets(LudoModule)
BuildTool.RegisterModules(LudoModule)
