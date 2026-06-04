add_requires("spirv-reflect")

target("Waterlily.Renderer", function()
    set_kind("shared")
    set_group("Engine")

    add_packages("spirv-reflect")

    add_deps("Waterlily.Core", "Waterlily.RHI", "Waterlily.Assets", "Waterlily.Scene")

    target_files_default({
        public = true
    })

    add_defines("WL_RENDERER_EXPORTS")
end)
