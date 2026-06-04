#pragma once

#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/RHIVulkan/RHIVulkanExports.hpp"

#include <vk_mem_alloc.h>

#define WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) Name

#define WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) PFN_##Name

#define WL_DEFINE_RHI_FUNCTION(Name) \
    static WL_RHI_VULKAN_API WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name);

#define WL_DEFINE_RHI_FUNCTION_IMPL(Name)    \
    WL_DEFINE_RHI_VULKAN_FUNCTION_TYPE(Name) \
    VulkanAPI::WL_DEFINE_RHI_VULKAN_FUNCTION_NAME(Name) = nullptr;

namespace Wl
{

    struct VulkanContext;

    class VulkanAPI
    {
    public:
        // Core Vulkan functions.
        WL_DEFINE_RHI_FUNCTION(vkGetInstanceProcAddr);
        WL_DEFINE_RHI_FUNCTION(vkEnumerateInstanceVersion);
        WL_DEFINE_RHI_FUNCTION(vkEnumerateInstanceExtensionProperties);
        WL_DEFINE_RHI_FUNCTION(vkEnumerateInstanceLayerProperties);
        WL_DEFINE_RHI_FUNCTION(vkCreateInstance);

        // Instance functions.
        WL_DEFINE_RHI_FUNCTION(vkGetDeviceProcAddr);
        WL_DEFINE_RHI_FUNCTION(vkDestroyInstance);
        WL_DEFINE_RHI_FUNCTION(vkEnumeratePhysicalDevices);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceProperties);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceProperties2);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFeatures);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFeatures2);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceFormatProperties);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
        WL_DEFINE_RHI_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
        WL_DEFINE_RHI_FUNCTION(vkEnumerateDeviceExtensionProperties);
        WL_DEFINE_RHI_FUNCTION(vkCreateDevice);
        WL_DEFINE_RHI_FUNCTION(vkDestroySurfaceKHR);

        // Device functions.
        WL_DEFINE_RHI_FUNCTION(vkGetDeviceQueue);
        WL_DEFINE_RHI_FUNCTION(vkDeviceWaitIdle);
        WL_DEFINE_RHI_FUNCTION(vkCreateCommandPool);
        WL_DEFINE_RHI_FUNCTION(vkDestroyCommandPool);
        WL_DEFINE_RHI_FUNCTION(vkDestroyDevice);
        WL_DEFINE_RHI_FUNCTION(vkCreateSwapchainKHR);
        WL_DEFINE_RHI_FUNCTION(vkDestroySwapchainKHR);
        WL_DEFINE_RHI_FUNCTION(vkGetSwapchainImagesKHR);
        WL_DEFINE_RHI_FUNCTION(vkCreateImage);
        WL_DEFINE_RHI_FUNCTION(vkCreateImageView);
        WL_DEFINE_RHI_FUNCTION(vkDestroyImage);
        WL_DEFINE_RHI_FUNCTION(vkDestroyImageView);
        WL_DEFINE_RHI_FUNCTION(vkCreateRenderPass);
        WL_DEFINE_RHI_FUNCTION(vkDestroyRenderPass);
        WL_DEFINE_RHI_FUNCTION(vkCreateFramebuffer);
        WL_DEFINE_RHI_FUNCTION(vkDestroyFramebuffer);
        WL_DEFINE_RHI_FUNCTION(vkGetImageMemoryRequirements);
        WL_DEFINE_RHI_FUNCTION(vkAllocateMemory);
        WL_DEFINE_RHI_FUNCTION(vkFreeMemory);
        WL_DEFINE_RHI_FUNCTION(vkAllocateCommandBuffers);
        WL_DEFINE_RHI_FUNCTION(vkFreeCommandBuffers);
        WL_DEFINE_RHI_FUNCTION(vkBeginCommandBuffer);
        WL_DEFINE_RHI_FUNCTION(vkEndCommandBuffer);
        WL_DEFINE_RHI_FUNCTION(vkResetCommandBuffer);
        WL_DEFINE_RHI_FUNCTION(vkBindImageMemory);
        WL_DEFINE_RHI_FUNCTION(vkCreateSemaphore);
        WL_DEFINE_RHI_FUNCTION(vkDestroySemaphore);
        WL_DEFINE_RHI_FUNCTION(vkCreateFence);
        WL_DEFINE_RHI_FUNCTION(vkDestroyFence);
        WL_DEFINE_RHI_FUNCTION(vkWaitForFences);
        WL_DEFINE_RHI_FUNCTION(vkAcquireNextImageKHR);
        WL_DEFINE_RHI_FUNCTION(vkResetFences);
        WL_DEFINE_RHI_FUNCTION(vkCreateDescriptorSetLayout);
        WL_DEFINE_RHI_FUNCTION(vkDestroyDescriptorSetLayout);
        WL_DEFINE_RHI_FUNCTION(vkCreateDescriptorPool);
        WL_DEFINE_RHI_FUNCTION(vkDestroyDescriptorPool);
        WL_DEFINE_RHI_FUNCTION(vkResetDescriptorPool);
        WL_DEFINE_RHI_FUNCTION(vkCreateShaderModule);
        WL_DEFINE_RHI_FUNCTION(vkDestroyShaderModule);
        WL_DEFINE_RHI_FUNCTION(vkCreateSampler);
        WL_DEFINE_RHI_FUNCTION(vkDestroySampler);
        WL_DEFINE_RHI_FUNCTION(vkCreateBuffer);
        WL_DEFINE_RHI_FUNCTION(vkDestroyBuffer);
        WL_DEFINE_RHI_FUNCTION(vkGetBufferMemoryRequirements);
        WL_DEFINE_RHI_FUNCTION(vkBindBufferMemory);
        WL_DEFINE_RHI_FUNCTION(vkMapMemory);
        WL_DEFINE_RHI_FUNCTION(vkUnmapMemory);
        WL_DEFINE_RHI_FUNCTION(vkFlushMappedMemoryRanges);
        WL_DEFINE_RHI_FUNCTION(vkCreatePipelineCache);
        WL_DEFINE_RHI_FUNCTION(vkDestroyPipelineCache);
        WL_DEFINE_RHI_FUNCTION(vkCreatePipelineLayout);
        WL_DEFINE_RHI_FUNCTION(vkDestroyPipelineLayout);
        WL_DEFINE_RHI_FUNCTION(vkCreateGraphicsPipelines);
        WL_DEFINE_RHI_FUNCTION(vkDestroyPipeline);
        WL_DEFINE_RHI_FUNCTION(vkCmdBindPipeline);
        WL_DEFINE_RHI_FUNCTION(vkAllocateDescriptorSets);
        WL_DEFINE_RHI_FUNCTION(vkFreeDescriptorSets);
        WL_DEFINE_RHI_FUNCTION(vkUpdateDescriptorSets);

        // Commands
        WL_DEFINE_RHI_FUNCTION(vkCmdPipelineBarrier);
        WL_DEFINE_RHI_FUNCTION(vkCmdBlitImage);
        WL_DEFINE_RHI_FUNCTION(vkCmdCopyBuffer);
        WL_DEFINE_RHI_FUNCTION(vkCmdCopyBufferToImage);
        WL_DEFINE_RHI_FUNCTION(vkCmdCopyImageToBuffer);
        WL_DEFINE_RHI_FUNCTION(vkCmdExecuteCommands);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetViewport);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetScissor);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetFrontFace);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetCullMode);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetStencilTestEnable);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetDepthTestEnable);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetDepthWriteEnable);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetStencilReference);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetStencilOp);
        WL_DEFINE_RHI_FUNCTION(vkCmdBeginRendering);
        WL_DEFINE_RHI_FUNCTION(vkCmdEndRendering);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetStencilCompareMask);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetStencilWriteMask);
        WL_DEFINE_RHI_FUNCTION(vkCmdClearColorImage);
        WL_DEFINE_RHI_FUNCTION(vkCmdClearDepthStencilImage);
        WL_DEFINE_RHI_FUNCTION(vkCmdSetPrimitiveTopology);
        WL_DEFINE_RHI_FUNCTION(vkCmdPushConstants);
        WL_DEFINE_RHI_FUNCTION(vkCmdBindDescriptorSets);
        WL_DEFINE_RHI_FUNCTION(vkCmdBindVertexBuffers);
        WL_DEFINE_RHI_FUNCTION(vkCmdBindIndexBuffer);
        WL_DEFINE_RHI_FUNCTION(vkCmdDraw);
        WL_DEFINE_RHI_FUNCTION(vkCmdDrawIndexed);
        WL_DEFINE_RHI_FUNCTION(vkCmdDrawIndexedIndirect);
        WL_DEFINE_RHI_FUNCTION(vkCmdBeginRenderPass);
        WL_DEFINE_RHI_FUNCTION(vkCmdEndRenderPass);

        // Queue
        WL_DEFINE_RHI_FUNCTION(vkQueueSubmit);
        WL_DEFINE_RHI_FUNCTION(vkQueueWaitIdle);
        WL_DEFINE_RHI_FUNCTION(vkQueuePresentKHR);
    };

    SharedPtr<DynamicLibrary> VulkanLibraryLoad();

    bool VulkanCoreAPILoad(SharedPtr<DynamicLibrary> library);

    bool VulkanInstanceAPILoad(VkInstance instance);

    bool VulkanDeviceAPILoad(VkDevice device);

}// namespace Wl
