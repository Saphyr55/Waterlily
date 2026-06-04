#include "Waterlily/RHIVulkan/vulkanPipelineCache.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

namespace Wl
{

    void VulkanPipelineCache::Create(SharedPtr<Stream> stream)
    {
        m_cacheStream = stream;

        uint64_t size = stream->GetSize();
        Array<uint8_t> data;
        data.Resize(size);
        stream->Read(data.data(), data.size());

        VkPipelineCacheCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        createInfo.initialDataSize = size;
        createInfo.pInitialData = data.data();

        VulkanContext& vkc = VulkanContextGet();
        WL_VULKAN_CHECK(
                VulkanAPI::vkCreatePipelineCache(vkc.Device, &createInfo, vkc.Allocator, &m_handke));
    }

    void VulkanPipelineCache::Destroy()
    {
        VulkanContext& vkc = VulkanContextGet();

        VulkanAPI::vkDestroyPipelineCache(vkc.Device, m_handke, vkc.Allocator);
    }

}// namespace Wl
