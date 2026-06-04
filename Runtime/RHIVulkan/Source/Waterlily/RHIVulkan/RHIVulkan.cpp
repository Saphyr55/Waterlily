#include "Waterlily/RHIVulkan/RHIVulkan.hpp"

#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

namespace Wl
{

    Array<VkExtensionProperties> VulkanRHI::GetInstanceExtensionProperties()
    {
        uint32_t count = 0;
        WL_VULKAN_CHECK(VulkanAPI::vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));

        Array<VkExtensionProperties> extensions;
        extensions.Resize(count);
        WL_VULKAN_CHECK(
                VulkanAPI::vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data()));

        return extensions;
    }

}// namespace Wl