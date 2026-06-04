target("Waterlily.Scene", function()
    set_kind("shared")
    set_group("Engine")

    add_deps("Waterlily.Core", "Waterlily.Assets", "Waterlily.Entity")

    target_files_default({
        public = true
    })

    add_defines("WL_SCENE_EXPORTS")
end)
