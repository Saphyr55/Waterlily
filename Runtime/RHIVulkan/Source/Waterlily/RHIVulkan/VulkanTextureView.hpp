#pragma once

#include "Waterlily/RHI/TextureView.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanTextureView : public RHITextureView
    {
    public:
        VkImageView& GetHandle()
        {
            return m_handle;
        }

        void Create(const RHITextureViewDescription& description, VkImageView image_view);
        void Create(const RHITextureViewDescription& description);

        void Destroy();

    public:
        VulkanTextureView()
            : m_handle(VK_NULL_HANDLE)
        {
        }

        ~VulkanTextureView() = default;

    private:
        VkImageView m_handle;
    };

}// namespace Wl
