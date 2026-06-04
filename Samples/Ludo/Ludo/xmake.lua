target("Ludo", function()
    set_kind("shared")
    set_group("Samples/Ludo")

    local deps = {
        "Waterlily.Core",
        "Waterlily.Engine",
        "Waterlily.RHI",
        "Waterlily.RHI.Vulkan",
        "Waterlily.Messaging",
        "Waterlily.Scene",
        "Waterlily.Renderer",
        "Waterlily.Entity",
        "Waterlily.Assets"
    }

    add_deps(unpack(deps))

    target_files_default({
        public = true
    })

    add_defines("WL_LUDO_EXPORTS")
end)
