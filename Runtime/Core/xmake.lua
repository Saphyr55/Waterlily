target("Waterlily.Core", function()
    set_kind("shared")
    set_group("Engine")

    add_packages("libsdl3", "lua")

    target_files_default({
        public = true
    })

    add_defines("WL_CORE_EXPORTS")
end)

target("Waterlily.Core.Tests", function()
    set_kind("binary")
    set_group("Engine.Tests")

    add_deps("Waterlily.Core")

    add_packages("catch2")

    add_includedirs("Tests")

    add_files("Tests/**.cpp")
end)

target("Waterlily.Core.Benchmarks", function()
    set_kind("binary")
    set_group("Engine.Benchmarks")
    set_optimize("none")

    add_deps("Waterlily.Core")

    add_includedirs("Benchmarks")

    add_files("Benchmarks/**.cpp")
    add_headerfiles("Benchmarks/**.hpp")
end)
