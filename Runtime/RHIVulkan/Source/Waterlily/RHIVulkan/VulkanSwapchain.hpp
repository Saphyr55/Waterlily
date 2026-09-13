#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    struct VulkanSwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        Array<VkSurfaceFormatKHR> SurfaceFormats;
        Array<VkPresentModeKHR> PresentModes;
    };

    class VulkanSwapchain : public RHISwapchain
    {
    public:
        virtual RHISwapchainAcquireResult& AcquireNextFrame(RHISemaphore* semaphore) override;

        virtual RHISwapchainAcquireResult& GetCurrentAcquireResult() override
        {
            return m_currentResult;
        }

        virtual uint32_t GetWidth() override;

        virtual uint32_t GetHeight() override;

        virtual RHIFormat GetFormat() override;

        virtual ArrayView<RHISwapchainBuffer> GetBuffers() const override
        {
            return m_buffers;
        }

        virtual const RHISwapchainBuffer& GetCurrentBuffer() const override
        {
            return m_buffers[m_currentResult.ImageIndex];
        }

        void Create();

        void Destroy();

    public:
        inline VkFormat GetVulkanFormat()
        {
            return m_format;
        }

        inline VkSwapchainKHR& GetHandle()
        {
            return m_handle;
        }

        inline VkExtent2D GetExtent()
        {
            return m_extent;
        }

        void SetExtent(VkExtent2D extent)
        {
            m_extent = extent;
        }

    public:
        VulkanSwapchain(VulkanContext& context, uint32_t width, uint32_t height, uint32_t imageCount);

    private:
        VulkanSwapchainSupportDetails QuerySupportDetails();

    private:
        Allocator* m_allocator;
        VulkanContext& m_context;
        RHISwapchainAcquireResult m_currentResult;
        VkFormat m_format;
        VkExtent2D m_extent;
        Array<VkImage> m_images;
        Array<RHISwapchainBuffer> m_buffers;
        VkSwapchainKHR m_handle = VK_NULL_HANDLE;
        uint32_t m_imageCount = 3;
    };

}// namespace Wl