
local RendererModule = BuildTool.DefaultTargetTemplate()

local XMakePackages = {
    "spirv-reflect",
}

add_requires("spirv-reflect 1.4.335")

RendererModule.Name = "Waterlily.Renderer"
RendererModule.Kind = "shared"
RendererModule.Group = "Engine"

RendererModule.XMakePackages = XMakePackages
RendererModule.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.RHI",
    "Waterlily.Assets",
    "Waterlily.Scene"
}

RendererModule.Defines = {
    "WL_RENDERER_EXPORTS"
}

BuildTool.RegisterTargets(RendererModule)
BuildTool.RegisterModules(RendererModule)
