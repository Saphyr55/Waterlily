-- Asset Conditioning Pipeline --

local ACPCore = BuildTool.DefaultTargetTemplate()

ACPCore.Name = "ACP.Core"
ACPCore.Kind = "shared"
ACPCore.Group = "Tools"

ACPCore.XMakePackages = {
    "tinygltf"
}

ACPCore.Deps = {
    "Waterlily.Core",
    "Waterlily.Renderer",
    "Waterlily.Assets",
    "stb"
}

ACPCore.Defines = {
    "WL_TOOLS_ACP_EXPORTS"
}

add_requires("tinygltf")

BuildTool.RegisterTargets(ACPCore)
BuildTool.RegisterModules(ACPCore)
