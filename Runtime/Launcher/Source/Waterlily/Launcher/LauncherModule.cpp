#include "Waterlily/Launcher/LauncherModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(LauncherModule, "Waterlily.Launcher");

    void LauncherModule::OnStartup()
    {
        WL_LOG_INFO("LauncherModule", "Launcher module started.");
    }

    void LauncherModule::OnShutdown()
    {
        WL_LOG_INFO("LauncherModule", "Launcher module stopped.");
    }

}// namespace Wl