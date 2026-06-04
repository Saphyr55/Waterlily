target("WACP", function()
    set_kind("binary")
    set_group("Tools")

    add_deps("Waterlily.Core", "Waterlily.Engine", "WACP.Core")

    add_files("Source/ACP/Main.cpp")

end)
