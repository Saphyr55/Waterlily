-- Asset Conditioning Pipeline --

local ACP = BuildTool.DefaultTargetTemplate()

ACP.Name = "WlTools.ACP"
ACP.Kind = "shared"
ACP.Group = "Tools"

ACP.XMakePackages = {
    "tinygltf"
}

add_requires(unpack(ACP.XMakePackages))

ACP.Deps = {
    "Waterlily.Core",
    "Waterlily.Renderer",
    "Waterlily.Assets",
    "stb"
}

ACP.Defines = {
    "WL_TOOLS_ACP_EXPORTS"
}

BuildTool.RegisterTargets(ACP)
