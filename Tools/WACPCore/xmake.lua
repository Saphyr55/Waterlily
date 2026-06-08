-- Asset Conditioning Pipeline --
add_requires("tinygltf")

target("WACP.Core", function()
    set_kind("shared")
    set_group("Tools")

    add_packages("tinygltf", {
        public = false
    })

    add_deps("Waterlily.Core", "Waterlily.Renderer", "Waterlily.Assets", "stb")

    add_includedirs("Source", {
        public = true
    })

    add_headerfiles("Source/**.hpp")
    add_files("Source/**.cpp")

    add_defines("WL_TOOLS_ACP_EXPORTS")
end)
