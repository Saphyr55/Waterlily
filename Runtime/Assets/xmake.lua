target("Waterlily.Assets", function()
    set_kind("shared")
    set_group("Engine")

    add_deps("Waterlily.Core")

    target_files_default({
        public = true
    })

    add_defines("WL_ASSETS_EXPORTS")
end)
