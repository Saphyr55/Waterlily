local LudoDevModule = BuildTool.DefaultTargetTemplate()

LudoDevModule.Name = "Ludo.Dev"
LudoDevModule.Kind = "shared"
LudoDevModule.Group = "Samples/Ludo"
LudoDevModule.Deps = {
    -- Ludo
    "Ludo",
    -- Runtime
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.RHI",
    "Waterlily.Messaging",
    "Waterlily.Scene",
    "Waterlily.Renderer",
    "Waterlily.Entity",
    "Waterlily.Assets",
    -- Tools
    "WlTools.ShaderCompiler"
}

LudoDevModule.Defines = {
    "WL_LUDO_DEV_EXPORTS"
}

LudoDevModule.XMakePackages = {
    "imgui"
}

add_requires(unpack(LudoDevModule.XMakePackages))

BuildTool.RegisterTargets(LudoDevModule)
BuildTool.RegisterModules(LudoDevModule)
