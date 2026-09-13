#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanPipeline.hpp"
#include "Waterlily/RHI/ComputePipeline.hpp"

namespace Wl
{

    struct VulkanContext;

    class VulkanComputePipeline : public RHIComputePipeline
    {
    public:
        void Create(const RHIComputePipelineDescription& description);

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
        VulkanComputePipeline() = default;
        ~VulkanComputePipeline() override = default;

    private:
        VulkanPipeline m_handle;
        RHIComputePipelineDescription m_description;
    };

}// namespace Wl
