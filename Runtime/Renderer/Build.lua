
local RendererModule = BuildTool.DefaultTargetTemplate()

local XMakePackages = {
    "spirv-reflect",
    "directxshadercompiler"
}

add_requires(unpack(XMakePackages))

RendererModule.Name = "Waterlily.Renderer"
RendererModule.Kind = "shared"
RendererModule.Group = "Engine"

RendererModule.XMakePackages = XMakePackages
RendererModule.Deps = {
    "Waterlily.Core",
    "Waterlily.RHI",
    "Waterlily.Assets",
    "Waterlily.Scene"
}

RendererModule.Defines = {
    "WL_RENDERER_EXPORTS"
}

BuildTool.RegisterTargets(RendererModule)
BuildTool.RegisterModules(RendererModule)
