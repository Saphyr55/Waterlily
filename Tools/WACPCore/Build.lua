-- Asset Conditioning Pipeline --

local WACPCore = BuildTool.DefaultTargetTemplate()

WACPCore.Name = "WACP.Core"
WACPCore.Kind = "shared"
WACPCore.Group = "Tools"

WACPCore.XMakePackages = {
    "tinygltf"
}

WACPCore.Deps = {
    "Waterlily.Core",
    "Waterlily.Renderer",
    "Waterlily.Assets",
    "stb"
}

WACPCore.Defines = {
    "WL_TOOLS_ACP_EXPORTS"
}

add_requires("tinygltf")

BuildTool.RegisterTargets(WACPCore)
