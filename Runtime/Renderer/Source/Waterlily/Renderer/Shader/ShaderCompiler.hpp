#pragma once

#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"

namespace Wl
{

    struct ShaderCompileInfo
    {
        StringRef Filepath;
        StringRef OutputFilepath;
        StringRef EntryPoint;
        Shader::Stage Stage;
        bool KeepIntermediateFile = true;
    };

    enum class ShaderCompileResult
    {
        Success = 0,
        Failed = 1,
        Unknown,
    };

    class WL_RENDERER_API IShaderCompiler
    {
    public:
        static SharedPtr<IShaderCompiler> Create(StringRef envPath);

        virtual ShaderCompileResult Compile(const ShaderCompileInfo& desc) = 0;

        virtual ~IShaderCompiler() = default;
    };

}// namespace Wl