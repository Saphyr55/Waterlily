#pragma once

#include "CompiledShader.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHIPipelineShaderStageCreateInfo
    {
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
        StringRef Name = "main";
        SPIRVShader Shader;
    };

    struct RHIShaderConstantRange
    {
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
        uint32_t Offset = 0;
        uint32_t Size = 128;
    };

    struct RHIPipelineViewportStateInformation
    {
        Viewport Viewport;
        Rect2D Scissor;
    };

    class RHIPipeline
    {
    public:
        enum class Type
        {
            Graphics,
            Compute,
        };

        virtual Type GetType() = 0;

        virtual ~RHIPipeline() = default;
    };

}// namespace Wl