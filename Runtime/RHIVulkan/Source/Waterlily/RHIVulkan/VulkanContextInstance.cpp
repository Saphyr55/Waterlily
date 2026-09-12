
#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHIVulkan/RHIVulkan.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vk_mem_alloc.h>

namespace Wl
{

    void VulkanInstanceCreate(VulkanContext& context)
    {
        Array<const char*> desiredExtensions = {
                VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        Array<VkExtensionProperties> availableExtensions = VulkanRHI::GetInstanceExtensionProperties();

        Array<StringRef> availableExtensionNames;
        availableExtensionNames.Resize(availableExtensions.size());

        auto propsToCStr = [](const VkExtensionProperties& properties) -> const char*
        {
            return properties.extensionName;
        };

        Wl::Transform(availableExtensions.begin(),
                      availableExtensions.end(),
                      availableExtensionNames.begin(),
                      propsToCStr);

        for (StringRef ext: availableExtensionNames)
        {
            WL_LOG_DEBUG("Vulkan", ext);
        }

        for (StringRef ext: desiredExtensions)
        {
            WL_CHECK_MSG(availableExtensionNames.Contains(ext), "Required Vulkan extension '%s' is not available.", ext);
        }

        Array<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

        VkApplicationInfo applicationInfo {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = "WaterlilyApplication";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "WaterlilyEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = context.VulkanAPIVersion;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(desiredExtensions.size());
        createInfo.ppEnabledExtensionNames = desiredExtensions.data();

        WL_LOG_INFO("Vulkan", "Creating Vulkan instance with the following extensions:");
        for (const char* extension: desiredExtensions)
        {
            WL_LOG_INFO("Vulkan", "  - %s", extension);
        }

        WL_LOG_INFO("Vulkan", "Using the following validation layers:");
        for (const char* layer: validationLayers)
        {
            WL_LOG_INFO("Vulkan", "  - %s", layer);
        }

        WL_VULKAN_CHECK(VulkanAPI::vkCreateInstance(&createInfo, context.Allocator, &context.Instance));

        WL_LOG_INFO("Vulkan", "Vulkan instance created successfully.");

        VulkanInstanceAPILoad(context.Instance);
    }

    void VulkanInstanceDestroy(VulkanContext& context)
    {
        WL_LOG_INFO("Vulkan", "Destroying Vulkan instance...");
        VulkanAPI::vkDestroyInstance(context.Instance, context.Allocator);
        context.Instance = VK_NULL_HANDLE;
        WL_LOG_INFO("Vulkan", "Vulkan instance destroyed.");
    }

}// namespace Wl