set_project("Waterlily")
set_version("0.0.1-dev")

set_languages("c++20")

local targetdir = "Build/Bin/$(plat)_$(arch)_$(mode)"
set_targetdir(targetdir)

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
    add_extrafiles("Assets/**")
end

add_moduledirs("Tools")

includes("Tools/BuildTool/BuildTool.lua")
includes("Tools/BuildTool/Tasks.lua")

-- Third Party -- 
BuildTool.IncludeTargets("ThirdParty/stb")
BuildTool.IncludeTargets("ThirdParty/nlohmann")

-- Runtime -- 
BuildTool.IncludeTargets("Runtime/Assets")
BuildTool.IncludeTargets("Runtime/Core")
BuildTool.IncludeTargets("Runtime/Scene")
BuildTool.IncludeTargets("Runtime/RHIVulkan")
BuildTool.IncludeTargets("Runtime/RHI")
BuildTool.IncludeTargets("Runtime/Renderer")
BuildTool.IncludeTargets("Runtime/Messaging")
BuildTool.IncludeTargets("Runtime/Launcher")
BuildTool.IncludeTargets("Runtime/Entity")
BuildTool.IncludeTargets("Runtime/Engine")

-- Sample sources --
BuildTool.IncludeTargets("Samples/Ludo/App")
BuildTool.IncludeTargets("Samples/Ludo/Ludo")

-- Tools sources --
BuildTool.IncludeTargets("Tools/WACP")
BuildTool.IncludeTargets("Tools/WACPCore")

-- Setup Targets --
BuildTool.SetupTargets();
