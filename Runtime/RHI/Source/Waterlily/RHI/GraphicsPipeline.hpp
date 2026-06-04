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

        RHIVertexBindingDescription() = default;
        RHIVertexBindingDescription(uint32_t in_binding,
                                    uint32_t in_stride,
                                    RHIVertexInputRate in_input_rate = RHIVertexInputRate::Vertex)
            : Binding(in_binding)
            , Stride(in_stride)
            , InputRate(in_input_rate)
        {
        }
    };

    struct RHIVertexAttributeDescription
    {
        uint32_t Binding = 0;
        uint32_t Location = 0;
        RHIFormat Format = RHIFormat::R32_FLOAT;
        uint32_t Offset = 0;

        RHIVertexAttributeDescription() = default;
        RHIVertexAttributeDescription(uint32_t in_binding, uint32_t in_location, RHIFormat in_format, uint32_t in_offset = 0)
            : Binding(in_binding)
            , Location(in_location)
            , Format(in_format)
            , Offset(in_offset)
        {
        }
    };

    struct RHIGraphicsPipelineShaderStageCreateInfo
    {
        StringRef Name = "main";
        SPIRVShader Shader;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
    };

    struct RHIGraphicsPipelineViewportStateInformation
    {
        Viewport Viewport;
        Rect2D Scissor;
    };

    struct RHIGraphicsPipelineVertexBindingInformation
    {
        Array<RHIVertexBindingDescription> Bindings;
        Array<RHIVertexAttributeDescription> Attributes;
    };

    struct RHIShaderConstantRange
    {
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
        uint32_t Offset = 0;
        uint32_t Size = 128;
    };

    struct RHIGraphicsPipelineDescription
    {
        RHICullModeFlags CullMode = RHICullModeFlags::None;
        RHIGraphicsPipelineShaderStageCreateInfo VertexShaderInfo = {};
        RHIGraphicsPipelineShaderStageCreateInfo FragmentShaderInfo = {};
        RHIGraphicsPipelineViewportStateInformation ViewportInfo = {};
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
            m_description.VertexShaderInfo = {.Name = name, .Shader = shader, .Stage = RHIShaderStage::Vertex};
            return *this;
        }

        RHIGraphicsPipelineDescriptionBuilder& WithFragmentShader(const SPIRVShader& shader, StringRef name = "main")
        {
            m_description.FragmentShaderInfo = {.Name = name, .Shader = shader, .Stage = RHIShaderStage::Fragment};
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
