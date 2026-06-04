#include "Waterlily/RHIVulkan/VulkanTexturePool.hpp"

namespace Wl
{

    void VulkanTexturePool::Init()
    {
    }

    RHITexture* VulkanTexturePool::Allocate(const RHITextureDescription& description)
    {
        return nullptr;
    }

    void VulkanTexturePool::Deallocate(RHITexture* texture)
    {
    }

    void VulkanTexturePool::Reset()
    {
    }

}// namespace Wl