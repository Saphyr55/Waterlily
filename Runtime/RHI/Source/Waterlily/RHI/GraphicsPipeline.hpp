#pragma once

#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHI/CompiledShader.hpp"
#include "Waterlily/RHI/Pipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHIVertexBindingDescription
    {
        uint32_t Binding = 0;
        uint32_t Stride = 0;
        RHIVertexInputRate InputRate = RHIVertexInputRate::Vertex;
    };

    struct RHIVertexAttributeDescription
    {
        uint32_t Binding = 0;
        uint32_t Location = 0;
        RHIFormat Format = RHIFormat::R32_FLOAT;
        uint32_t Offset = 0;
    };

    struct RHIGraphicsPipelineVertexBindingInformation
    {
        Array<RHIVertexBindingDescription> Bindings;
        Array<RHIVertexAttributeDescription> Attributes;
    };

    struct RHIGraphicsPipelineDescription
    {
        RHIFrontFace FrontFace = RHIFrontFace::CounterClockwise;
        RHICullModeFlags CullMode = RHICullModeFlags::None;
        RHIPipelineShaderStageCreateInfo VertexShaderInfo = {};
        RHIPipelineShaderStageCreateInfo FragmentShaderInfo = {};
        RHIPipelineViewportStateInformation ViewportInfo = {};
        RHIGraphicsPipelineVertexBindingInformation VertexBindingInfo = {};
        Array<RHIShaderResourceGroupLayout*> SRGLayouts;
        Array<RHIShaderConstantRange> ShaderConstantRanges;
        RHIRenderPass* RenderPass = nullptr;
    };

    class RHIGraphicsPipelineDescriptionBuilder
    {
    public:
        RHIGraphicsPipelineDescriptionBuilder& WithCullMode(RHICullModeFlags mode)
        {
            m_description.CullMode = mode;
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithVertexShader(const SPIRVShader& shader, StringRef name = "main")
        {
            m_description.VertexShaderInfo = {RHIShaderStage::Vertex, name, shader};
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithFragmentShader(const SPIRVShader& shader, StringRef name = "main")
        {
            m_description.FragmentShaderInfo = {RHIShaderStage::Fragment, name, shader};
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithViewport(const Viewport& viewport, const Rect2D& scissor)
        {
            m_description.ViewportInfo = {viewport, scissor};
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithVertexBindings(const Array<RHIVertexBindingDescription>& bindings,
                                                                  const Array<RHIVertexAttributeDescription>& attributes)
        {
            m_description.VertexBindingInfo = {bindings, attributes};
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithSRGLayout(
                const Array<RHIShaderResourceGroupLayout*>& layouts)
        {
            m_description.SRGLayouts = layouts;
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& with_shader_constant_ranges(const Array<RHIShaderConstantRange>& ranges)
        {
            m_description.ShaderConstantRanges = ranges;
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithRenderPass(RHIRenderPass* renderPass)
        {
            m_description.RenderPass = renderPass;
            return *this;
        }

        const RHIGraphicsPipelineDescription& Build() const
        {
            return m_description;
        }

    private:
        RHIGraphicsPipelineDescription m_description = {};
    };

    class RHIGraphicsPipeline : public RHIPipeline
    {
    public:
        virtual Type GetType() override final
        {
            return Type::Graphics;
        }

        virtual ~RHIGraphicsPipeline() = default;
    };

}// namespace Wl
