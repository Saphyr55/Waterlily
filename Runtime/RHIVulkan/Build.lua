local RHIVulkanModule = BuildTool.DefaultTargetTemplate()

local XMakePackages = {
    "vulkan-headers",
    "vulkan-memory-allocator"
}

add_requires(unpack(XMakePackages))

RHIVulkanModule.Name = "Waterlily.RHI.Vulkan"
RHIVulkanModule.Kind = "shared"
RHIVulkanModule.Group = "Engine"

RHIVulkanModule.Deps = {
    "Waterlily.Core",
    "Waterlily.RHI"
}

RHIVulkanModule.PublicIncludes = {}
RHIVulkanModule.PrivateIncludes = {
    "Source/"
}

RHIVulkanModule.XMakePackages = XMakePackages

RHIVulkanModule.Defines = {
    "WL_RHI_VULKAN_EXPORTS"
}

RHIVulkanModule.Callback = function()
    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end

BuildTool.RegisterTargets(RHIVulkanModule)
BuildTool.RegisterModules(RHIVulkanModule)
