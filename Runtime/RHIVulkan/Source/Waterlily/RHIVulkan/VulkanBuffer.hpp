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

        virtual usize GetSize() override;

        virtual void Bind() override;

        virtual void* Map(usize offset = 0, usize size = 0) override;

        virtual void Unmap() override;

        virtual void Update(const void* data, usize size, usize offset = 0) override;

        void FlushWhenIsHost(usize offset, usize size);

        VkMemoryRequirements GetMemoryRequirements();

        Array<uint32> GetQueueFamilyIndices();

        void Create(const RHIBufferDescription& description);
        void Destroy();

        inline usize GetID() const
        {
            return m_id;
        }

        inline void SetID(usize id)
        {
            m_id = id;
        }

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
        usize m_id = 0;
    };

}// namespace Wl