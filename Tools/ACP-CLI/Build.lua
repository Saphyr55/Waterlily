local ACP = {}

ACP.Name = "WlTools.ACP.CLI"
ACP.Kind = "binary"
ACP.Group = "Tools"
ACP.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.Launcher",
    "WlTools.ACP"
}

ACP.Sources = {
    "Source/WlTools/ACP/Main.cpp"
}

BuildTool.RegisterTargets(ACP)
