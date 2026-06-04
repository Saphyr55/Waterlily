#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanPipeline.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"

namespace Wl
{

    struct VulkanContext;

    class VulkanGraphicsPipeline : public RHIGraphicsPipeline
    {//< FIXEME: Future use with cache and handle.
    public:
        void Create(const RHIGraphicsPipelineDescription& description);

        void Destroy();

        const VulkanPipeline& GetHandle() const
        {
            return m_handle;
        }

        VulkanPipeline& GetHandle()
        {
            return m_handle;
        }

    public:
        VulkanGraphicsPipeline();

    private:
        VulkanPipeline m_handle;
        RHIGraphicsPipelineDescription m_description;
    };

}// namespace Wl
