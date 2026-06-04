#include "Waterlily/Launcher/LauncherModule.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(LauncherModule, "Waterlily.Launcher");

    void LauncherModule::OnLoad()
    {
        LLOG_INFO("[LauncherModule]", "Launcher module loaded.");
    }

    void LauncherModule::OnStartup()
    {
        LLOG_INFO("[LauncherModule]", "Launcher module started.");
    }

    void LauncherModule::OnShutdown()
    {
        LLOG_INFO("[LauncherModule]", "Launcher module stopped.");
    }

    void LauncherModule::OnUnload()
    {
        LLOG_INFO("[LauncherModule]", "Launcher module unloaded.");
    }

}// namespace Wl