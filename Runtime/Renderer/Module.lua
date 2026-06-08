local XMakePackages = {
    "spirv-reflect",
    "directxshadercompiler"
}

add_requires(unpack(XMakePackages))

local RendererModule = {}
RendererModule.Name = "Waterlily.Renderer"
RendererModule.Kind = "shared"
RendererModule.Group = "Engine"

RendererModule.XMakePackages = XMakePackages
RendererModule.Deps = {
"Waterlily.Core", "Waterlily.RHI", "Waterlily.Assets", "Waterlily.Scene"
}

RendererModule.PublicIncludes = {
    "Source"
}
RendererModule.Headers = {
    "Source/**.hpp"
}
RendererModule.Sources = {
    "Source/**.cpp"
}
RendererModule.PrivateDefines = {
    "WL_RENDERER_EXPORTS"
}

BuildTool.RegisterTargets(RendererModule)
