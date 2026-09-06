#pragma once

#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/RHI/Types.hpp"

#include <slang.h>

namespace Wl
{

    struct ShaderBinding
    {
        String Name;
        uint32_t Binding;
        uint32_t Group;
        uint32_t Count = 1;
        RHIShaderResourceType Type;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
    };

    struct ShaderVertexInput
    {
        RHIFormat Format;
        uint32_t Location;
        uint32_t Stride;
    };

    void PrintProgramLayout(slang::ProgramLayout* programLayout);

}// namespace Wl