#include "Waterlily/RHIVulkan/VulkanTexture.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

namespace Wl
{

    VkMemoryRequirements VulkanTexture::GetMemoryRequirements()
    {
        VkMemoryRequirements memoryRequirements;
        VulkanAPI::vkGetImageMemoryRequirements(VulkanContextGet().Device, m_handle, &memoryRequirements);
        return memoryRequirements;
    }

    void VulkanTexture::Bind()
    {
        WL_VULKAN_CHECK(vmaBindImageMemory(VulkanContextGet().VmaAllocator, m_allocation, m_handle));
    }

    void VulkanTexture::Create(const RHITextureDescription& desc)
    {
        m_description = desc;

        VulkanContext& context = VulkanContextGet();

        VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
        switch (m_description.Dimension)
        {
            case RHITextureDimension::Dim3D:
                imageType = VK_IMAGE_TYPE_3D;
                break;
            case RHITextureDimension::Dim2D:
                imageType = VK_IMAGE_TYPE_2D;
                break;
            case RHITextureDimension::Dim1D:
                imageType = VK_IMAGE_TYPE_1D;
                break;
            default:
                imageType = VK_IMAGE_TYPE_MAX_ENUM;
                break;
        }

        Array<uint32_t> queueFamillies = VulkanQueryQueueFamilyIndices(context);

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = VK_NULL_HANDLE;
        imageCreateInfo.imageType = imageType;
        imageCreateInfo.format = VulkanFormatGet(desc.Format);
        imageCreateInfo.arrayLayers = desc.Layers;
        imageCreateInfo.mipLevels = desc.MipLevels;
        imageCreateInfo.extent.width = Math::Max(desc.Width, 2u);
        imageCreateInfo.extent.height = Math::Max(desc.Height, 2u);
        imageCreateInfo.extent.depth = desc.Depth;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VulkanTextureLayoutGet(desc.Layout);
        imageCreateInfo.usage = VulkanTextureUsageGet(desc.Usage);
        imageCreateInfo.sharingMode = VulkanSharingModeGet(desc.SharingMode);
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        if (RHISharingMode::Private == desc.SharingMode)
        {
            imageCreateInfo.pQueueFamilyIndices = nullptr;
            imageCreateInfo.queueFamilyIndexCount = 0;
        }
        else if (RHISharingMode::Shared == desc.SharingMode)
        {
            imageCreateInfo.pQueueFamilyIndices = queueFamillies.data();
            imageCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamillies.size());
        }

        VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
        vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

        WL_VULKAN_CHECK(vmaCreateImage(context.VmaAllocator,
                                       &imageCreateInfo,
                                       &vmaAllocationCreateInfo,
                                       &m_handle,
                                       &m_allocation,
                                       nullptr));
    }

    void VulkanTexture::Destroy()
    {
        VulkanContext& context = VulkanContextGet();
        vmaDestroyImage(context.VmaAllocator, m_handle, m_allocation);
    }

}// namespace Wl