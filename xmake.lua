set_project("Waterlily")
set_version("0.0.1-dev")

set_languages("c++20")

set_targetdir("Build/Bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {
    outputdir = ".xmake"
})

if is_mode("debug") then
    add_defines("WL_DEBUG")
end

add_requires("libsdl3", "catch2", "lua")

target("Assets")
do
    set_kind("phony")
    set_group("Assets")

    add_headerfiles("Assets/**")
end

includes("Tools/BuildTool/BuildTool.lua")

BuildTool.IncludeModule("Runtime/Core")
BuildTool.IncludeModule("Runtime/Scene")
BuildTool.IncludeModule("Runtime/RHIVulkan")
BuildTool.IncludeModule("Runtime/RHI")
BuildTool.IncludeModule("Runtime/Renderer")
BuildTool.IncludeModule("Runtime/Messaging")
BuildTool.IncludeModule("Runtime/Launcher")
BuildTool.IncludeModule("Runtime/Entity")
BuildTool.IncludeModule("Runtime/Engine")
BuildTool.IncludeModule("Runtime/Assets")

-- Sample sources --
BuildTool.IncludeModule("Samples/Ludo/App")
BuildTool.IncludeModule("Samples/Ludo/Ludo")

-- Tools sources --
includes("Tools/WACP")
includes("Tools/WACPCore")
includes("Tools/WACPDump")

-- Third Party -- 
includes("ThirdParty")

BuildTool.SetupTargets();
