target("WACP.Dump", function()
    set_kind("binary")
    set_group("Tools")

    add_deps("Waterlily.Core", "Waterlily.Assets", "WACP.Core")
    
    add_files("main.cpp")

end)
