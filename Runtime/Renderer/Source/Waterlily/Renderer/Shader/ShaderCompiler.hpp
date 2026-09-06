#pragma once

#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"

namespace Wl
{

    struct ShaderCompileSlangDesc
    {
        StringRef EnvPath;
        StringRef SlangFilepath;
        StringRef OutputFilepath;
        StringRef EntryPoint;
        Shader::Stage Stage;
        bool KeepSpvFile = true;
    };


    class WL_RENDERER_API ShaderCompiler
    {
    public:
        static bool CompileSlang(const ShaderCompileSlangDesc& desc);
    };

}// namespace Wl