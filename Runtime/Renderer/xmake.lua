add_requires("spirv-reflect", "directxshadercompiler")

target("Waterlily.Renderer")
do
    set_kind("shared")
    set_group("Engine")

    add_packages("spirv-reflect", "directxshadercompiler")

    add_deps("Waterlily.Core", "Waterlily.RHI", "Waterlily.Assets", "Waterlily.Scene")

    target_files_default({
        public = true
    })

    add_defines("WL_RENDERER_EXPORTS")
end
