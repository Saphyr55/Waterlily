local XMakePackages = {
    "vulkan-headers",
    "vulkan-memory-allocator"
}

add_requires(unpack(XMakePackages))

local RHIVulkanModule = {}

RHIVulkanModule.Name = "Waterlily.RHI.Vulkan"
RHIVulkanModule.Kind = "shared"
RHIVulkanModule.Group = "Engine"

RHIVulkanModule.Deps = {
    "Waterlily.Core",
    "Waterlily.RHI"
}

RHIVulkanModule.PrivateIncludes = {
    "Source/"
}
RHIVulkanModule.Sources = {
    "Source/**.cpp"
}
RHIVulkanModule.Headers = {
    "Source/**.hpp"
}

RHIVulkanModule.XMakePackages = XMakePackages

RHIVulkanModule.PrivateDefines = {
    "WL_RHI_VULKAN_EXPORTS"
}

RHIVulkanModule.Callback = function()
    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end

BuildTool.RegisterTargets(RHIVulkanModule)