target("Waterlily.Entity", function()
    set_kind("shared")
    set_group("Engine")

    add_deps("Waterlily.Core")

    target_files_default({
        public = true
    })

    add_defines("WL_ENTITY_EXPORTS")
end)

target("Waterlily.Entity.Tests", function()
    set_kind("binary")
    set_group("Engine.Tests")

    add_deps("Waterlily.Core", "Waterlily.Entity")

    add_packages("catch2")

    add_includedirs("tests")

    add_files("tests/**.cpp")

    add_defines("WL_ENTITY_EXPORTS")
end)