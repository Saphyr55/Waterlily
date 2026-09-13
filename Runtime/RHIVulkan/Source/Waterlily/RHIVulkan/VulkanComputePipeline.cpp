#include "VulkanComputePipeline.hpp"
#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"
#include "Waterlily/RHIVulkan/VulkanShaderModule.hpp"

namespace Wl
{

    void VulkanComputePipeline::Create(const RHIComputePipelineDescription& description)
    {
        m_description = description;

        VulkanContext& context = VulkanContextGet();

        SPIRVShader& computeCompiledShader = m_description.ComputeShaderInfo.Shader;
        VulkanShaderModule computeShaderModule(computeCompiledShader.GetByteCode());

        computeShaderModule.Create();

        VkPipelineShaderStageCreateInfo computeStageCreateInfo = {};
        computeStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeStageCreateInfo.module = computeShaderModule.GetHandle();
        computeStageCreateInfo.pName = m_description.ComputeShaderInfo.Name;

        auto toVulkanDescriptorSetLayout = [](RHIShaderResourceGroupLayout* l) -> VkDescriptorSetLayout
        {
            return static_cast<VulkanShaderResourceGroupLayout*>(l)->GetHandle();
        };

        Array<VkDescriptorSetLayout> layouts;
        layouts.Resize(m_description.SRGLayouts.size());

        Wl::Transform(m_description.SRGLayouts.begin(),
                      m_description.SRGLayouts.end(),
                      layouts.begin(),
                      toVulkanDescriptorSetLayout);

        auto toVulkanPushConstantRange = [](const RHIShaderConstantRange& range) -> VkPushConstantRange
        {
            VkPushConstantRange vulkanPushConstantRange = {};
            vulkanPushConstantRange.stageFlags = VulkanShaderStageGet(range.Stage);
            vulkanPushConstantRange.offset = range.Offset;
            vulkanPushConstantRange.size = range.Size;
            return vulkanPushConstantRange;
        };

        Array<VkPushConstantRange> pushConstantRanges;
        pushConstantRanges.Resize(m_description.ShaderConstantRanges.GetSize());

        Wl::Transform(m_description.ShaderConstantRanges.begin(),
                      m_description.ShaderConstantRanges.end(),
                      pushConstantRanges.begin(),
                      toVulkanPushConstantRange);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = layouts.GetSize();
        pipelineLayoutCreateInfo.pSetLayouts = layouts.GetData();
        pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRanges.GetSize();
        pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.GetData();

        WL_VULKAN_CHECK(VulkanAPI::vkCreatePipelineLayout(context.Device,
                                                          &pipelineLayoutCreateInfo,
                                                          context.Allocator,
                                                          &m_handle.GetPipelineLayout()));

        VkComputePipelineCreateInfo computePipelineCreateInfo = {};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;// TODO: Configurable
        computePipelineCreateInfo.basePipelineIndex = -1;             // TODO: Configurable
        computePipelineCreateInfo.layout = m_handle.GetPipelineLayout();
        computePipelineCreateInfo.stage = computeStageCreateInfo;

        WL_VULKAN_CHECK(VulkanAPI::vkCreateComputePipelines(context.Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, context.Allocator, &m_handle.GetPipeline()));

        computeShaderModule.Destroy();
    }

    void VulkanComputePipeline::Destroy()
    {
        m_handle.Destroy();
    }

}// namespace Wl