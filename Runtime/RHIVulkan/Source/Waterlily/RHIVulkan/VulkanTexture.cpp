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
        VulkanContext& context = VulkanContextGet();
        VkMemoryRequirements memory_requirements;
        VulkanAPI::vkGetImageMemoryRequirements(context.Device, m_handle, &memory_requirements);

        return memory_requirements;
    }

    void VulkanTexture::Bind()
    {
        VulkanContext& context = VulkanContextGet();
        WL_VULKAN_CHECK(vmaBindImageMemory(context.VmaAllocator, m_allocation, m_handle));
    }

    void VulkanTexture::Create(const RHITextureDescription& desc)
    {
        m_description = desc;
        VulkanContext& context = VulkanContextGet();
        VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
        switch (m_description.Dimension)
        {
            case RHITextureDimension::Dim3D:
                image_type = VK_IMAGE_TYPE_3D;
                break;
            case RHITextureDimension::Dim2D:
                image_type = VK_IMAGE_TYPE_2D;
                break;
            case RHITextureDimension::Dim1D:
                image_type = VK_IMAGE_TYPE_1D;
                break;
            default:
                image_type = VK_IMAGE_TYPE_MAX_ENUM;
                break;
        }

        Array<uint32_t> queue_famillies = VulkanQueryQueueFamilyIndices(context);

        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.pNext = VK_NULL_HANDLE;
        image_create_info.imageType = image_type;
        image_create_info.format = VulkanFormatGet(desc.Format);
        image_create_info.arrayLayers = desc.Layers;
        image_create_info.mipLevels = desc.MipLevels;
        image_create_info.extent.width = Math::Max(desc.Width, 2u);
        image_create_info.extent.height = Math::Max(desc.Height, 2u);
        image_create_info.extent.depth = desc.Depth;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = VulkanTextureUsageGet(desc.Usage);
        image_create_info.sharingMode = VulkanSharingModeGet(desc.SharingMode);
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;

        if (RHISharingMode::Private == desc.SharingMode)
        {
            image_create_info.pQueueFamilyIndices = nullptr;
            image_create_info.queueFamilyIndexCount = 0;
        }
        else if (RHISharingMode::Shared == desc.SharingMode)
        {
            image_create_info.pQueueFamilyIndices = queue_famillies.data();
            image_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_famillies.size());
        }

        VmaAllocationCreateInfo vma_allocation_create_info = {};
        vma_allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

        WL_VULKAN_CHECK(vmaCreateImage(context.VmaAllocator,
                                       &image_create_info,
                                       &vma_allocation_create_info,
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