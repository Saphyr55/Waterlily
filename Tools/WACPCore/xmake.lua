-- Asset Conditioning Pipeline --
add_requires("tinygltf")

target("WACP.Core", function()
    set_kind("shared")
    set_group("Tools")

    add_packages("tinygltf", {
        public = false
    })

    add_deps("Waterlily.Core", "Waterlily.Renderer", "Waterlily.Assets", "stb")

    target_files_default({
        public = true
    })

    add_defines("LTOOLS_ACP_EXPORTS")
end)
