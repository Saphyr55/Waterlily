#pragma once

#include "Waterlily/RHI/Sampler.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanSampler : public RHISampler
    {
    public:
        VkSampler& GetHandle()
        {
            return m_handle;
        }

        void Create(const RHISamplerDescription& description);
        void Destroy();

    public:
        VulkanSampler() = default;
        ~VulkanSampler();

    private:
        VkSampler m_handle = VK_NULL_HANDLE;
    };

}// namespace Wl