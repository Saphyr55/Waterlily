#pragma once

#include "Waterlily/Core/Modules/ModuleRegistry.hpp"

namespace Wl
{

    class CoreModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl
