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
    add_defines("WL_SID_STRING_REGISTRY_ENABLE")
end

add_requires("libsdl3", "catch2", "lua")

function target_files_default(param)
    add_includedirs("Source", param)

    add_headerfiles("Source/**.hpp")
    add_files("Source/**.cpp")
end

target("Assets") 
do 
    set_kind("phony")
    set_group("Assets")

    add_headerfiles("Assets/**")
end

-- Engine sources --
includes("Runtime/Core")
includes("Runtime/Messaging")
includes("Runtime/Engine")
includes("Runtime/Launcher")
includes("Runtime/RHI")
includes("Runtime/RHIVulkan")
includes("Runtime/Scene")
includes("Runtime/Renderer")
includes("Runtime/Entity")
includes("Runtime/Assets")

-- Tools sources --
includes("Tools/WACP")
includes("Tools/WACPCore")
includes("Tools/WACPDump")

-- Sample sources --
includes("Samples/Ludo/Ludo")
includes("Samples/Ludo/App")

-- Third Party -- 
includes("ThirdParty")
