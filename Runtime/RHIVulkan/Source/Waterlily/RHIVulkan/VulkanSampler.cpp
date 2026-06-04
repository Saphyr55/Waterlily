#include "Waterlily/RHIVulkan/VulkanSampler.hpp"
#include "Waterlily/RHI/Sampler.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    void VulkanSampler::Create(const RHISamplerDescription& description)
    {
        m_description = description;

        VulkanContext& context = VulkanContextGet();

        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VulkanFilterGet(description.MagFilter);
        samplerCreateInfo.minFilter = VulkanFilterGet(description.MinFilter);
        samplerCreateInfo.addressModeU = VulkanAddressModeGet(description.AddressModeU);
        samplerCreateInfo.addressModeV = VulkanAddressModeGet(description.AddressModeV);
        samplerCreateInfo.addressModeW = VulkanAddressModeGet(description.AddressModeW);
        samplerCreateInfo.anisotropyEnable = context.PhysicalDeviceInfo.Features.samplerAnisotropy;
        samplerCreateInfo.maxAnisotropy = 16.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = static_cast<VkBool32>(description.IsNormalize);
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = m_description.MinLOD;
        samplerCreateInfo.maxLod = m_description.MaxLOD;

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateSampler(context.Device, &samplerCreateInfo, context.Allocator, &m_handle));
    }

    void VulkanSampler::Destroy()
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            VulkanAPI::vkDestroySampler(VulkanContextGet().Device, m_handle, VulkanContextGet().Allocator);
            m_handle = VK_NULL_HANDLE;
        }
    }

    VulkanSampler::~VulkanSampler()
    {
    }

}// namespace Wl