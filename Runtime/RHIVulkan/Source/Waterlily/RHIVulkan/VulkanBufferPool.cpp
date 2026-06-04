#include "Waterlily/RHIVulkan/VulkanBufferPool.hpp"

namespace Wl
{

    void VulkanBufferPool::InitPool()
    {
    }

    RHIBuffer* VulkanBufferPool::Allocate(const RHIBufferDescription& description)
    {
        return nullptr;
    }

    void VulkanBufferPool::Deallocate(RHIBuffer* buffer)
    {
    }

    void VulkanBufferPool::Reset()
    {
    }

    void VulkanBufferPool::DestroyBuffer(VulkanBuffer* vulkan_buffer)
    {
    }

}// namespace Wl