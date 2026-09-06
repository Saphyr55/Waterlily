local ACP = {}

ACP.Name = "ACP"
ACP.Kind = "binary"
ACP.Group = "Tools"
ACP.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.Launcher",
    "ACP.Core"
}

ACP.Sources = {
    "Source/ACP/Main.cpp"
}

BuildTool.RegisterTargets(ACP)
