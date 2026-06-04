#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#define WL_LOAD_RHI_CORE_FUNCTION(Name)                                        \
    VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) =                      \
            reinterpret_cast<WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name)>(        \
                    ::Wl::DynamicLibraryLoader::LoadFunction(library, #Name)); \
    if (VulkanAPI::Name == nullptr)                                            \
    {                                                                          \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan function: " #Name);      \
    }

#define WL_LOAD_RHI_INSTANCE_FUNCTION(Name)                                                              \
    VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) =                                                \
            (WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name))VulkanAPI::vkGetInstanceProcAddr(instance, #Name); \
    if (VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) == nullptr)                                  \
    {                                                                                                    \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan instance function: " #Name);                       \
    }

#define WL_LOAD_RHI_DEVICE_FUNCTION(Name)                                                                              \
    VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) =                                                              \
            reinterpret_cast<WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name)>(VulkanAPI::vkGetDeviceProcAddr(device, #Name)); \
    if (VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) == nullptr)                                                \
    {                                                                                                                  \
        LLOG_ERROR("[Vulkan]", "Failed to load Vulkan device function: " #Name);                                       \
    }

namespace Wl
{

    // Core Vulkan functions.
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetInstanceProcAddr);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceVersion);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceExtensionProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateInstanceLayerProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateInstance);

    // Instance functions.
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetDeviceProcAddr);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyInstance);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEnumeratePhysicalDevices);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceProperties2);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFeatures);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFeatures2);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceMemoryProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceQueueFamilyProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceFormatProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceFormatsKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfacePresentModesKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetPhysicalDeviceSurfaceSupportKHR)
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEnumerateDeviceExtensionProperties);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateDevice);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroySurfaceKHR);

    // Device functions.
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetDeviceQueue);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDeviceWaitIdle);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateCommandPool);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyCommandPool);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDevice);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateSwapchainKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroySwapchainKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetSwapchainImagesKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateImageView);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyImageView);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateRenderPass);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyRenderPass);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateFramebuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyFramebuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetImageMemoryRequirements);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkAllocateMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkFreeMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkAllocateCommandBuffers);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkFreeCommandBuffers);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkBeginCommandBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkEndCommandBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkResetCommandBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkBindImageMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateSemaphore);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroySemaphore);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateFence);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyFence);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkWaitForFences);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkAcquireNextImageKHR);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkResetFences);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateDescriptorSetLayout);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDescriptorSetLayout);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateDescriptorPool);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyDescriptorPool);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkResetDescriptorPool);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateShaderModule);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyShaderModule);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateSampler);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroySampler);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkGetBufferMemoryRequirements);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkBindBufferMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkMapMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkUnmapMemory);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkFlushMappedMemoryRanges);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreatePipelineCache);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyPipelineCache);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreatePipelineLayout);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyPipelineLayout);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCreateGraphicsPipelines);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkDestroyPipeline);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindPipeline);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkAllocateDescriptorSets);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkFreeDescriptorSets);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkUpdateDescriptorSets);

    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdPipelineBarrier);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBlitImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyBufferToImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdCopyImageToBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdExecuteCommands);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetViewport);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetScissor);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetFrontFace);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetCullMode);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilTestEnable);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetDepthTestEnable);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetDepthWriteEnable);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilReference);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilOp);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBeginRendering);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdEndRendering);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilCompareMask);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetStencilWriteMask);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdClearColorImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdClearDepthStencilImage);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdSetPrimitiveTopology);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdPushConstants);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindDescriptorSets);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindVertexBuffers);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBindIndexBuffer);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdDraw);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdDrawIndexed);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdDrawIndexedIndirect);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdBeginRenderPass);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkCmdEndRenderPass);

    WL_DEFINE_RHI_FUNCTION_IMPL(vkQueueSubmit);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkQueueWaitIdle);
    WL_DEFINE_RHI_FUNCTION_IMPL(vkQueuePresentKHR);

    SharedPtr<DynamicLibrary> VulkanLibraryLoad()
    {
#if defined(_WIN32)
        return DynamicLibraryLoader::Load("vulkan-1.dll");
#else
        return SharedPtr<DynamicLibrary>("libvulkan.so");
#endif
    }

    bool VulkanCoreAPILoad(SharedPtr<DynamicLibrary> library)
    {
        WL_LOAD_RHI_CORE_FUNCTION(vkGetInstanceProcAddr);
        WL_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceVersion);
        WL_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceExtensionProperties);
        WL_LOAD_RHI_CORE_FUNCTION(vkEnumerateInstanceLayerProperties);
        WL_LOAD_RHI_CORE_FUNCTION(vkCreateInstance);

        return true;
    }

    bool VulkanInstanceAPILoad(VkInstance instance)
    {
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetDeviceProcAddr);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkDestroyInstance);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkEnumeratePhysicalDevices);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties2);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFeatures2);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkEnumerateDeviceExtensionProperties);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkCreateDevice);
        WL_LOAD_RHI_INSTANCE_FUNCTION(vkDestroySurfaceKHR);

        return true;
    }

    bool VulkanDeviceAPILoad(VkDevice device)
    {
        WL_LOAD_RHI_DEVICE_FUNCTION(vkGetDeviceQueue);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDeviceWaitIdle);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateCommandPool);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyCommandPool);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDevice);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateSwapchainKHR);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroySwapchainKHR);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateImageView);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyImageView);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateRenderPass);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyRenderPass);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateFramebuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyFramebuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkAllocateMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkFreeMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkAllocateCommandBuffers);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkFreeCommandBuffers);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkBeginCommandBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkEndCommandBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkResetCommandBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkBindImageMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateSemaphore);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroySemaphore);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateFence);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyFence);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkWaitForFences);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkAcquireNextImageKHR);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkResetFences);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateDescriptorPool);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyDescriptorPool);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkResetDescriptorPool);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateShaderModule);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyShaderModule);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateSampler);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroySampler);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkBindBufferMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkMapMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkUnmapMemory);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreatePipelineCache);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyPipelineCache);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreatePipelineLayout);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyPipelineLayout);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkDestroyPipeline);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindPipeline);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkAllocateDescriptorSets);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkFreeDescriptorSets);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkUpdateDescriptorSets);

        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdPipelineBarrier);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBlitImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdExecuteCommands);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetViewport);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetScissor);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetFrontFace);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetCullMode);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilTestEnable);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetDepthTestEnable);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetDepthWriteEnable);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilReference);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilOp);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBeginRendering);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdEndRendering);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilCompareMask);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetStencilWriteMask);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdClearColorImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdSetPrimitiveTopology);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdPushConstants);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdDraw);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdDrawIndexed);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdDrawIndexedIndirect);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdBeginRenderPass);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkCmdEndRenderPass);

        WL_LOAD_RHI_DEVICE_FUNCTION(vkQueueSubmit);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkQueueWaitIdle);
        WL_LOAD_RHI_DEVICE_FUNCTION(vkQueuePresentKHR);

        return true;
    }

}// namespace Wl
