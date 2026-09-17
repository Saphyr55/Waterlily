#pragma once

#include "LudoDevExports.hpp"
#include "Waterlily/Core/Modules/Module.hpp"
#include "WlTools/ShaderCompiler/ShaderCompiler.hpp"

using namespace Wl;

namespace Ludo
{

    class LUDO_DEV_API LudoDevModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        bool CompileShaders();

    private:
        SharedPtr<IShaderCompiler> m_shaderCompiler;
    };

}// namespace Ludo
