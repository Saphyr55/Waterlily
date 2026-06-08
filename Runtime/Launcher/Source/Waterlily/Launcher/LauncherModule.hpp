#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Launcher/LauncherExports.hpp"

namespace Wl
{

    class WL_LAUNCHER_API LauncherModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

        virtual ~LauncherModule() = default;
    };

}// namespace Wl