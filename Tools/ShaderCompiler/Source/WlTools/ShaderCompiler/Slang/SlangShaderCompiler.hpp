#pragma once

#include "WlTools/ShaderCompiler/ShaderCompiler.hpp"

#include <slang-com-ptr.h>
#include <slang.h>

namespace Wl
{

    class WL_TOOLS_SHADER_COMPILER_API SlangShaderCompiler : public IShaderCompiler
    {
    public:

        virtual ShaderCompileResult Compile(const ShaderCompileInfo& desc) override;

    private:
        static SlangStage ShaderStageToSlangStage(Shader::Stage stage);
        ShaderCompileResult SaveSpvFile(const ShaderCompileInfo& desc, StringRef spvFilePath, Slang::ComPtr<slang::IBlob> kernelBlob);

    public:
        SlangShaderCompiler(StringRef envPath);
        ~SlangShaderCompiler();

    private:
        Slang::ComPtr<slang::IGlobalSession> m_globalSession;
        Slang::ComPtr<slang::ISession> m_session;
        String m_envPath;
    };

}// namespace WlTools