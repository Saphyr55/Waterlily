#include "Waterlily/RHIVulkan/VulkanSwapchain.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"
#include "Waterlily/RHIVulkan/VulkanSync.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"
#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    uint32_t VulkanSwapchainCountImage(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t imageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        return imageCount;
    }

    VkExtent2D VulkanChooseSwapchainExtent(VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = capabilities.currentExtent;
        }

        VkExtent2D min = capabilities.minImageExtent;
        VkExtent2D max = capabilities.maxImageExtent;

        extent.width = Math::Clamp(extent.width, Math::Max(2u, min.width), max.width);
        extent.height = Math::Clamp(extent.height, Math::Max(2u, min.height), max.height);

        return extent;
    }

    VkPresentModeKHR VulkanChoosePresentMode(const Array<VkPresentModeKHR>& availablePresentModes)
    {
        if (availablePresentModes.Contains(VK_PRESENT_MODE_MAILBOX_KHR))
        {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkSurfaceFormatKHR VulkanChooseSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableSurfaceFormats)
    {
        auto isPriorityFormat = [](const VkSurfaceFormatKHR& available_format) -> bool
        {
            return available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                   available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        };

        if (const VkSurfaceFormatKHR* surfaceFormat = availableSurfaceFormats.GetIf(isPriorityFormat))
        {
            return *surfaceFormat;
        }

        return availableSurfaceFormats[0];
    }

    VulkanSwapchain::VulkanSwapchain(VulkanContext& context, uint32_t width, uint32_t height, uint32_t image_count)
        : m_context(context)
        , m_extent({width, height})
        , m_format(VK_FORMAT_B8G8R8A8_SRGB)
        , m_imageCount(image_count)
    {
    }

    RHIFormat VulkanSwapchain::GetFormat()
    {
        return RHIFormatGet(m_format);
    }

    VulkanSwapchainSupportDetails VulkanSwapchain::QuerySupportDetails()
    {
        VulkanSwapchainSupportDetails swapchainSupportDetails = {};

        VkPhysicalDevice physicalDevice = m_context.PhysicalDevice;
        VkSurfaceKHR surface = m_context.Surface->GetHandle();

        WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physicalDevice, surface, &swapchainSupportDetails.Capabilities));

        uint32_t formatCount = 0;
        WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface, &formatCount, nullptr));

        if (formatCount != 0)
        {
            swapchainSupportDetails.SurfaceFormats.Resize(formatCount);
            WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice, surface, &formatCount, swapchainSupportDetails.SurfaceFormats.data()));
        }

        uint32_t presentModeCount = 0;
        WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface, &presentModeCount, nullptr));

        if (presentModeCount != 0)
        {
            swapchainSupportDetails.PresentModes.Resize(presentModeCount);
            WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice, surface, &presentModeCount, swapchainSupportDetails.PresentModes.data()));
        }

        return swapchainSupportDetails;
    }

    void VulkanSwapchain::Create()
    {
        VulkanSwapchainSupportDetails swapchainSupportDetails = QuerySupportDetails();

        bool isSwapchainAdequate =
                !swapchainSupportDetails.SurfaceFormats.empty() && !swapchainSupportDetails.PresentModes.empty();

        VkSurfaceFormatKHR surfaceFormat = VulkanChooseSurfaceFormat(swapchainSupportDetails.SurfaceFormats);
        VkPresentModeKHR presentMode = VulkanChoosePresentMode(swapchainSupportDetails.PresentModes);
        VkExtent2D extent = VulkanChooseSwapchainExtent(m_extent, swapchainSupportDetails.Capabilities);
        uint32_t minImageCount = VulkanSwapchainCountImage(swapchainSupportDetails.Capabilities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_context.Surface->GetHandle();
        swapchainCreateInfo.minImageCount = minImageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        // TODO: use VK_IMAGE_USAGE_TRANSFER_DST_BIT mask for post-processing for the future.
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t graphicsQueueIndex = m_context.PhysicalDeviceInfo.GraphicsQueueIndex;
        uint32_t presentQueueIndex = m_context.PhysicalDeviceInfo.PresentQueueIndex;
        uint32_t queueFamillies[] = {graphicsQueueIndex, presentQueueIndex};

        if (queueFamillies[0] != queueFamillies[1])
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamillies;
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;    // Optional
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;// Optional
        }

        swapchainCreateInfo.preTransform = swapchainSupportDetails.Capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateSwapchainKHR(m_context.Device, &swapchainCreateInfo, m_context.Allocator, &m_handle));

        uint32_t imageCount = 0;
        VulkanAPI::vkGetSwapchainImagesKHR(m_context.Device, m_handle, &imageCount, nullptr);

        if (m_imageCount >= imageCount)
        {
            m_imageCount = imageCount;
        }

        m_images.Resize(imageCount);
        m_textureViews.Reserve(imageCount);

        VulkanAPI::vkGetSwapchainImagesKHR(m_context.Device, m_handle, &imageCount, m_images.data());

        m_extent = extent;
        m_format = surfaceFormat.format;

        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImage image = m_images[i];

            VulkanTextureView* textureView = Wl::New(DefaultAllocator::GetInstance(), VulkanTextureView());

            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = image;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_format;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            WL_VULKAN_CHECK(VulkanAPI::vkCreateImageView(
                    m_context.Device, &imageViewCreateInfo, m_context.Allocator, &textureView->GetHandle()));

            m_textureViews.Append(textureView);
        }

        WL_CHECK(m_textureViews.size() == m_images.size());
    }

    void VulkanSwapchain::Destroy()
    {
        for (RHITextureView* textureView: m_textureViews)
        {
            VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(textureView);
            vulkanTextureView->Destroy();
        }

        m_textureViews.Clear();
        VulkanAPI::vkDestroySwapchainKHR(m_context.Device, m_handle, m_context.Allocator);
    }

    RHISwapchainAcquireResult& VulkanSwapchain::AcquireNextFrame(RHISemaphore* semaphore)
    {
        VulkanSemaphore* frameAvailableSemaphore = static_cast<VulkanSemaphore*>(semaphore);

        VkResult acquireNextImageResult = VulkanAPI::vkAcquireNextImageKHR(m_context.Device,
                                                                           m_handle,
                                                                           UINT64_MAX,
                                                                           frameAvailableSemaphore->GetHandle(),
                                                                           VK_NULL_HANDLE,
                                                                           &m_currentResult.ImageIndex);
        switch (acquireNextImageResult)
        {
            case VK_SUCCESS: {
                m_currentResult.IsSuccess = true;
                return m_currentResult;
            }
            case VK_SUBOPTIMAL_KHR: {
                m_currentResult.IsSuccess = true;
                m_currentResult.IsSuboptimal = true;
                return m_currentResult;
            }
            case VK_NOT_READY:
                m_currentResult.IsSuccess = false;
                m_currentResult.IsNotReady = true;
                return m_currentResult;
            case VK_ERROR_OUT_OF_DATE_KHR: {
                m_currentResult.IsSuccess = false;
                m_currentResult.IsOutOfDate = true;
                return m_currentResult;
            }
            default: {
                return m_currentResult;
            }
        }

        return m_currentResult;
    }

    uint32_t VulkanSwapchain::GetWidth()
    {
        return m_extent.width;
    }

    uint32_t VulkanSwapchain::GetHeight()
    {
        return m_extent.height;
    }

}// namespace Wl