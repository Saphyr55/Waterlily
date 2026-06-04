#pragma once

#include "Waterlily/Core/Modules/ModuleRegistry.hpp"

namespace Wl
{

    class CoreModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;
    };

}// namespace Wl
