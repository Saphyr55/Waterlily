#include "Waterlily/RHIVulkan/VulkanGraphicsPipeline.hpp"

#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/RHI/CompiledShader.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderPass.hpp"
#include "Waterlily/RHIVulkan/VulkanShaderModule.hpp"

#include <vk_mem_alloc.h>

namespace Wl
{

    static constexpr auto VulkanBindingMap =
            [](const RHIVertexBindingDescription& description) -> VkVertexInputBindingDescription
    {
        VkVertexInputBindingDescription vulkanDescription;
        vulkanDescription.binding = description.Binding;
        vulkanDescription.stride = description.Stride;
        vulkanDescription.inputRate = VulkanVertexInputRateGet(description.InputRate);
        return vulkanDescription;
    };

    static constexpr auto VulkanAttributeMap =
            [](const RHIVertexAttributeDescription& description) -> VkVertexInputAttributeDescription
    {
        VkVertexInputAttributeDescription vulkanDescription;
        vulkanDescription.binding = description.Binding;
        vulkanDescription.location = description.Location;
        vulkanDescription.format = VulkanFormatGet(description.Format);
        vulkanDescription.offset = description.Offset;
        return vulkanDescription;
    };

    VulkanGraphicsPipeline::VulkanGraphicsPipeline() = default;

    void VulkanGraphicsPipeline::Create(const RHIGraphicsPipelineDescription& description)
    {
        VulkanContext& context = VulkanContextGet();
        m_description = description;

        SPIRVShader& vertexCompiledShader = m_description.VertexShaderInfo.Shader;
        VulkanShaderModule vertexShaderModule(vertexCompiledShader.GetByteCode());

        SPIRVShader& fragmentCompiledShader = m_description.FragmentShaderInfo.Shader;
        VulkanShaderModule fragmentShaderModule(fragmentCompiledShader.GetByteCode());

        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
        pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        vertexShaderModule.Create();
        fragmentShaderModule.Create();

        Array<VkVertexInputBindingDescription> vertexBindings;
        vertexBindings.Resize(m_description.VertexBindingInfo.Bindings.GetSize());

        Wl::Transform(m_description.VertexBindingInfo.Bindings.begin(),
                      m_description.VertexBindingInfo.Bindings.end(),
                      vertexBindings.begin(),
                      VulkanBindingMap);

        Array<VkVertexInputAttributeDescription> vertexAttributes;
        vertexAttributes.Resize(m_description.VertexBindingInfo.Attributes.GetSize());

        Wl::Transform(m_description.VertexBindingInfo.Attributes.begin(),
                      m_description.VertexBindingInfo.Attributes.end(),
                      vertexAttributes.begin(),
                      VulkanAttributeMap);

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexBindings.size();
        vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindings.data();
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributes.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

        VkPipelineRasterizationStateCreateInfo pipelineRasterizerStateCreateInfo = {};
        pipelineRasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineRasterizerStateCreateInfo.depthClampEnable = VK_FALSE;
        pipelineRasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        pipelineRasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineRasterizerStateCreateInfo.lineWidth = 1.0f;

        VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
        if (m_description.CullMode == RHICullModeFlags::Back)
        {
            cullMode = VK_CULL_MODE_BACK_BIT;
        }
        else if (m_description.CullMode == RHICullModeFlags::Front)
        {
            cullMode = VK_CULL_MODE_FRONT_BIT;
        }
        else if (m_description.CullMode == RHICullModeFlags::None)
        {
            cullMode = VK_CULL_MODE_NONE;
        }

        pipelineRasterizerStateCreateInfo.cullMode = cullMode;
        pipelineRasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipelineRasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;
        pipelineRasterizerStateCreateInfo.depthBiasConstantFactor = 0.0f;// Optional
        pipelineRasterizerStateCreateInfo.depthBiasClamp = 0.0f;         // Optional
        pipelineRasterizerStateCreateInfo.depthBiasSlopeFactor = 0.0f;   // Optional

        VkPipelineMultisampleStateCreateInfo pipelineMultisamplingStateCreateInfo = {};
        pipelineMultisamplingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineMultisamplingStateCreateInfo.sampleShadingEnable = VK_FALSE;
        pipelineMultisamplingStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineMultisamplingStateCreateInfo.minSampleShading = 1.0f;         // Optional
        pipelineMultisamplingStateCreateInfo.pSampleMask = nullptr;           // Optional
        pipelineMultisamplingStateCreateInfo.alphaToCoverageEnable = VK_FALSE;// Optional
        pipelineMultisamplingStateCreateInfo.alphaToOneEnable = VK_FALSE;     // Optional

        VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
        vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageCreateInfo.module = vertexShaderModule.GetHandle();
        vertexShaderStageCreateInfo.pName = m_description.VertexShaderInfo.Name;

        VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
        fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageCreateInfo.module = fragmentShaderModule.GetHandle();
        fragmentShaderStageCreateInfo.pName = m_description.FragmentShaderInfo.Name;

        FixedArray<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStateCreateInfo = {
                vertexShaderStageCreateInfo,
                fragmentShaderStageCreateInfo};

        Array<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo pipelineDymanicStateCreateInfo = {};
        pipelineDymanicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDymanicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.GetSize());
        pipelineDymanicStateCreateInfo.pDynamicStates = dynamicStates.GetData();

        VkViewport viewport = {};
        viewport.x = m_description.ViewportInfo.Viewport.X;
        viewport.y = m_description.ViewportInfo.Viewport.Y;
        viewport.width = m_description.ViewportInfo.Viewport.Width;
        viewport.height = m_description.ViewportInfo.Viewport.Height;
        viewport.minDepth = m_description.ViewportInfo.Viewport.MinDepth;
        viewport.maxDepth = m_description.ViewportInfo.Viewport.MaxDepth;

        int32_t offset_x = m_description.ViewportInfo.Scissor.X;
        int32_t offset_y = m_description.ViewportInfo.Scissor.Y;
        VkRect2D scissor = {};
        scissor.offset = {offset_x, offset_y};
        scissor.extent = {
                static_cast<uint32_t>(m_description.ViewportInfo.Scissor.Width),
                static_cast<uint32_t>(m_description.ViewportInfo.Scissor.Height),
        };

        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
        pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipelineViewportStateCreateInfo.viewportCount = 1;
        pipelineViewportStateCreateInfo.pViewports = &viewport;
        pipelineViewportStateCreateInfo.scissorCount = 1;
        pipelineViewportStateCreateInfo.pScissors = &scissor;

        VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
        pipelineColorBlendAttachmentState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;            // Optional
        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
        pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;            // Optional

        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
        pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;// Optional
        pipelineColorBlendStateCreateInfo.attachmentCount = 1;
        pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
        pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;// Optional
        pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;// Optional
        pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;// Optional
        pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;// Optional

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

        VulkanRenderPass* renderPass = static_cast<VulkanRenderPass*>(m_description.RenderPass);

        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = pipelineShaderStateCreateInfo.GetSize();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pStages = pipelineShaderStateCreateInfo.GetData();
        graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizerStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisamplingStateCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;
        graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &pipelineDymanicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = m_handle.GetPipelineLayout();
        graphicsPipelineCreateInfo.renderPass = renderPass->GetHandle();
        graphicsPipelineCreateInfo.subpass = 0;                        // TODO: Configurable
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;// TODO: Configurable
        graphicsPipelineCreateInfo.basePipelineIndex = -1;             // TODO: Configurable

        WL_VULKAN_CHECK(VulkanAPI::vkCreateGraphicsPipelines(context.Device,
                                                             VK_NULL_HANDLE,
                                                             1,
                                                             &graphicsPipelineCreateInfo,
                                                             context.Allocator,
                                                             &m_handle.GetPipeline()));

        vertexShaderModule.Destroy();
        fragmentShaderModule.Destroy();
    }

    void VulkanGraphicsPipeline::Destroy()
    {
        VulkanContext& context = VulkanContextGet();

        VulkanAPI::vkDestroyPipeline(context.Device, m_handle.GetPipeline(), context.Allocator);
        VulkanAPI::vkDestroyPipelineLayout(context.Device, m_handle.GetPipelineLayout(), context.Allocator);
    }

}// namespace Wl