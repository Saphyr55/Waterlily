#pragma once

#include "ShaderCompilerExports.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/RHI/Types.hpp"

#include <slang.h>

namespace Wl
{

    struct ShaderBinding
    {
        String Name;
        uint32 Binding;
        uint32 Group;
        uint32 Count = 1;
        RHIShaderResourceType Type;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
    };

    struct ShaderVertexInput
    {
        RHIFormat Format;
        uint32 Location;
        uint32 Stride;
    };

    WL_TOOLS_SHADER_COMPILER_API void PrintProgramLayout(slang::ProgramLayout* programLayout);

}// namespace Wl