#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Launcher/LauncherExports.hpp"

namespace Wl
{

    class WL_LAUNCHER_API LauncherModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl