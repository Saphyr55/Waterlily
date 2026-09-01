#pragma once

#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"

namespace Wl
{

    class WL_RENDERER_API SPIRVShaderCompiler
    {
    public:
        static bool CompileSlang(StringRef inputFilepath, StringRef outputFilepath, StringRef entryPoint, Shader::Stage stage, bool keepSpvFile = true);

        static bool CompileHLSL(StringRef inputFilepath, StringRef outputFilepath, StringRef entryPoint, Shader::Stage stage);
    };

}// namespace Wl