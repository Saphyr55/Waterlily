#include "Waterlily/Launcher/LauncherModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(LauncherModule, "Waterlily.Launcher");

    void LauncherModule::OnLoad()
    {
        WL_LOG_INFO("[LauncherModule]", "Launcher module loaded.");
    }

    void LauncherModule::OnStartup()
    {
        WL_LOG_INFO("[LauncherModule]", "Launcher module started.");
    }

    void LauncherModule::OnShutdown()
    {
        WL_LOG_INFO("[LauncherModule]", "Launcher module stopped.");
    }

    void LauncherModule::OnUnload()
    {
        WL_LOG_INFO("[LauncherModule]", "Launcher module unloaded.");
    }

}// namespace Wl