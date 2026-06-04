add_requires("vulkan-headers", "vulkan-memory-allocator")

target("Waterlily.RHI.Vulkan", function()
    set_kind("shared")
    set_group("Engine")

    add_deps("Waterlily.Core", "Waterlily.RHI")

    add_includedirs("Source", {
        public = true
    })

    add_headerfiles("Source/**.hpp")
    add_files("Source/**.cpp")

    add_packages("vulkan-headers", "vulkan-memory-allocator", {
        public = false
    })

    add_defines("WL_RHI_VULKAN_EXPORTS")

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end

end)
