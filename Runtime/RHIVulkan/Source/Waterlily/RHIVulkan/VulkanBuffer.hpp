#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Types.hpp"

#include <vk_mem_alloc.h>

namespace Wl
{

    class VulkanBuffer : public RHIBuffer
    {
    public:
        virtual RHIBufferUsageFlags GetUsage() override;

        virtual RHISharingMode GetSharingMode() override;

        virtual size_t GetSize() override;

        virtual void Bind() override;

        virtual void* Map(size_t offset = 0, size_t size = 0) override;

        virtual void Unmap() override;

        virtual void Update(const void* data, size_t size, size_t offset = 0) override;

        void FlushWhenIsHost(size_t offset, size_t size);

        VkMemoryRequirements GetMemoryRequirements();

        Array<uint32_t> GetQueueFamilyIndices();

        void Create(const RHIBufferDescription& description);
        void Destroy();

        VkBuffer GetHandle();

    public:
        VulkanBuffer() = default;
        ~VulkanBuffer() = default;

    private:
        RHIBufferDescription m_description = {};
        VkDeviceMemory memory_ = VK_NULL_HANDLE;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        void* m_mapped = nullptr;
    };

}// namespace Wl