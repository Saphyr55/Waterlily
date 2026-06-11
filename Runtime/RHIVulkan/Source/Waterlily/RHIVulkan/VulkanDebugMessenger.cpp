#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                                void* userData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                WL_LOG_INFO("Vulkan", callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                WL_LOG_INFO("Vulkan", callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                WL_LOG_WARN("Vulkan", callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                WL_LOG_ERROR("Vulkan", callbackData->pMessage);
                break;
            default:
                WL_LOG_FATAL("Vulkan", "Unknown message severity.");
                break;
        }

        return VK_FALSE;
    }

    void VulkanDebugMessengerCreate(VulkanContext& context)
    {
        WL_LOG_INFO("Vulkan", "Initializing Vulkan debug messenger...");

        PFN_vkCreateDebugUtilsMessengerEXT fvkCreateDebugUtilsMessengerEXT =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                        VulkanAPI::vkGetInstanceProcAddr(context.Instance, "vkCreateDebugUtilsMessengerEXT"));
        WL_CHECK(fvkCreateDebugUtilsMessengerEXT);

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = &VulkanDebugMessengerCallback;
        createInfo.pUserData = nullptr;

        fvkCreateDebugUtilsMessengerEXT(context.Instance, &createInfo, context.Allocator, &context.DebugUtilsMessenger);
        WL_CHECK(context.DebugUtilsMessenger != VK_NULL_HANDLE);

        WL_LOG_INFO("Vulkan", "Vulkan debug messenger initialized.");
    }

    void VulkanDebugMessengerDestroy(VulkanContext& context)
    {
        WL_LOG_INFO("Vulkan", "Destroying Vulkan debug messenger...");

        // Destroy the debug messenger
        PFN_vkDestroyDebugUtilsMessengerEXT fvkDestroyDebugUtilsMessengerEXT =
                reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                        VulkanAPI::vkGetInstanceProcAddr(context.Instance, "vkDestroyDebugUtilsMessengerEXT"));
        WL_CHECK(fvkDestroyDebugUtilsMessengerEXT);

        if (context.DebugUtilsMessenger != VK_NULL_HANDLE)
        {
            fvkDestroyDebugUtilsMessengerEXT(context.Instance, context.DebugUtilsMessenger, context.Allocator);
            context.DebugUtilsMessenger = VK_NULL_HANDLE;
            WL_LOG_INFO("Vulkan", "Vulkan debug messenger destroyed.");
        }
        else
        {
            WL_LOG_WARN("Vulkan", "No Vulkan debug messenger to destroy.");
        }
    }

}// namespace Wl