#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Engine/engineExports.hpp"

namespace Wl
{

    class WL_ENGINE_API EngineModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

        virtual ~EngineModule() = default;
    };

}// namespace Wl