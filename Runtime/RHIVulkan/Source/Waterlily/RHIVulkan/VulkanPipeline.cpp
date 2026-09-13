#include "VulkanPipeline.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

namespace Wl
{

    void VulkanPipeline::Destroy()
    {
        VulkanContext& context = VulkanContextGet();

        VulkanAPI::vkDestroyPipeline(context.Device, m_pipeline, context.Allocator);
        VulkanAPI::vkDestroyPipelineLayout(context.Device, m_pipelineLayout, context.Allocator);
    }

}// namespace Wl