#pragma once

#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    class WL_RENDERER_API SPIRVShaderCompiler
    {
    public:
        enum class Stage : uint8_t
        {
            Vertex,
            Fragment,
            Compute,
        };

        static bool CompileGLSL(StringRef inputFilepath, StringRef outputFilepath, Stage stage);
        static bool CompileHLSL(StringRef inputFilepath, StringRef outputFilepath, Stage stage);

    };

}// namespace Wl