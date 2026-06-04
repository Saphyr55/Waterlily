#pragma once

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanPipeline
    {
    public:
        VkPipeline& GetPipeline()
        {
            return m_pipeline;
        };

        VkPipelineLayout& GetPipelineLayout()
        {
            return m_pipelineLayout;
        };

    protected:
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    };

}// namespace Wl