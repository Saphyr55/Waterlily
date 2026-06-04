#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/PipelineCache.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class Stream;

    class VulkanPipelineCache : public RHIPipelineCache
    {
    public:
        VkPipelineCache& GetHandle()
        {
            return m_handke;
        }

        void Create(SharedPtr<Stream> cacheStream);

        void Destroy();

        VulkanPipelineCache()
            : m_handke(VK_NULL_HANDLE)
            , m_cacheStream()
        {
        }

    private:
        SharedPtr<Stream> m_cacheStream;
        VkPipelineCache m_handke;
    };

}// namespace Wl
