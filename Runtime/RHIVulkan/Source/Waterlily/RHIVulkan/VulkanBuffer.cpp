
#include "Waterlily/RHIVulkan/VulkanBuffer.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    VkMemoryRequirements VulkanBuffer::GetMemoryRequirements()
    {
        VulkanContext& context = VulkanContextGet();

        VkMemoryRequirements memoryRequirements;
        VulkanAPI::vkGetBufferMemoryRequirements(context.Device, m_buffer, &memoryRequirements);
        return memoryRequirements;
    }

    Array<uint32_t> VulkanBuffer::GetQueueFamilyIndices()
    {
        VulkanContext& context = VulkanContextGet();

        Array<uint32_t> indices;
        if (m_description.SharingMode == RHISharingMode::Shared)
        {
            if ((m_description.Usage & RHIBufferUsageFlags::TransferDst) == RHIBufferUsageFlags::TransferDst ||
                (m_description.Usage & RHIBufferUsageFlags::TransferSrc) == RHIBufferUsageFlags::TransferSrc)
            {
                indices = VulkanQueryQueueFamilyIndices(context);
            }
        }
        return indices;
    }

    void VulkanBuffer::Create(const RHIBufferDescription& description)
    {
        m_description = description;

        VulkanContext& context = VulkanContextGet();

        Array<uint32_t> indices = GetQueueFamilyIndices();

        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = VK_NULL_HANDLE;
        bufferCreateInfo.queueFamilyIndexCount = indices.GetSize();
        bufferCreateInfo.pQueueFamilyIndices = indices.GetData();
        bufferCreateInfo.size = m_description.Size;
        bufferCreateInfo.usage = VulkanBufferUsageGet(m_description.Usage);
        bufferCreateInfo.sharingMode = VulkanSharingModeGet(m_description.SharingMode);

        VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
        vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        if (m_description.MemoryUsage == RHIMemoryUsage::Host)
        {
            vmaAllocationCreateInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            vmaAllocationCreateInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        WL_VULKAN_CHECK(vmaCreateBuffer(context.VmaAllocator,
                                        &bufferCreateInfo,
                                        &vmaAllocationCreateInfo,
                                        &m_buffer,
                                        &m_allocation,
                                        nullptr));
    }

    void VulkanBuffer::Destroy()
    {
        VulkanContext& context = VulkanContextGet();
        if (m_mapped)
        {
            Unmap();
        }
        vmaDestroyBuffer(context.VmaAllocator, m_buffer, m_allocation);
    }

    void VulkanBuffer::Bind()
    {
        VulkanContext& context = VulkanContextGet();
        WL_VULKAN_CHECK(vmaBindBufferMemory(context.VmaAllocator, m_allocation, m_buffer));
    }

    void* VulkanBuffer::Map(size_t offset, size_t size)
    {
        WL_CHECK_MSG(offset + size <= m_description.Size, "Update range exceeds buffer size.");

        VulkanContext& context = VulkanContextGet();

        WL_VULKAN_CHECK(vmaMapMemory(context.VmaAllocator, m_allocation, &m_mapped));

        FlushWhenIsHost(offset, size);

        return static_cast<uint8_t*>(m_mapped) + offset;
    }

    void VulkanBuffer::Unmap()
    {
        VulkanContext& context = VulkanContextGet();
        vmaUnmapMemory(context.VmaAllocator, m_allocation);
        m_mapped = nullptr;
    }

    void VulkanBuffer::Update(const void* source, size_t size, size_t offset)
    {
        WL_CHECK_MSG(source, "Source data must not be null.");

        VulkanContext& context = VulkanContextGet();

        void* destination = Map(offset, size);
        LLOG_ERROR_AND_RETURN_WHEN(!destination, "[Vulkan]", "Failed to map buffer memory for update.");

        Memory::Copy(destination, source, size);
        FlushWhenIsHost(offset, size);
    }

    void VulkanBuffer::FlushWhenIsHost(size_t offset, size_t size)
    {
        VulkanContext& context = VulkanContextGet();
        VkMemoryPropertyFlags properties = VulkanMemoryUsageGet(m_description.MemoryUsage);
        if (properties == (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
        {
            vmaFlushAllocation(context.VmaAllocator, m_allocation, offset, size);
        }
    }

    RHISharingMode VulkanBuffer::GetSharingMode()
    {
        return m_description.SharingMode;
    }

    size_t VulkanBuffer::GetSize()
    {
        return m_description.Size;
    }

    RHIBufferUsageFlags VulkanBuffer::GetUsage()
    {
        return m_description.Usage;
    }

    VkBuffer VulkanBuffer::GetHandle()
    {
        return m_buffer;
    }

}// namespace Wl