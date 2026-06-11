local WACP = {}

WACP.Name = "WACP"
WACP.Kind = "binary"
WACP.Group = "Tools"
WACP.Deps = {
    "Waterlily.Core",
    "Waterlily.Engine",
    "Waterlily.Launcher",
    "WACP.Core"
}

WACP.Sources = {
    "Source/ACP/Main.cpp"
}

BuildTool.RegisterTargets(WACP)
