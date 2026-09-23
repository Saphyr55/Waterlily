#include "ShaderCompiler.hpp"

#include "WlTools/ShaderCompiler/SlangShaderCompiler.hpp"

namespace Wl
{

    SharedPtr<IShaderCompiler> IShaderCompiler::Create(StringRef envPath)
    {
        return MakeShared<SlangShaderCompiler>(envPath);
    }

}// namespace Wl