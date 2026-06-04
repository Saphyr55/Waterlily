#pragma once

#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanTexture : public RHITexture
    {
    public:
        VkMemoryRequirements GetMemoryRequirements();

        void Create(const RHITextureDescription& description);

        void Destroy();

        virtual void Bind() override;

        VkImage& GetHandle()
        {
            return m_handle;
        }

        const VkImage& GetHandle() const
        {
            return m_handle;
        }

    public:
        VulkanTexture() = default;
        virtual ~VulkanTexture() override = default;

    private:
        VkImage m_handle;
        VmaAllocation m_allocation;
        VkDeviceMemory m_memory;
    };

}// namespace Wl