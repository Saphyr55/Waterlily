#include "Waterlily/Core/coreModule.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(CoreModule, "Waterlily.Core");

    void CoreModule::OnLoad()
    {
        LLOG_INFO("[CoreModule]", "Core module loaded.");
    }

    void CoreModule::OnStartup()
    {
        LLOG_INFO("[CoreModule]", "Core module started.");
    }

    void CoreModule::OnShutdown()
    {
        LLOG_INFO("[CoreModule]", "Core module stopped.");
    }

    void CoreModule::OnUnload()
    {
        LLOG_INFO("[CoreModule]", "Core module unloaded.");
    }

}// namespace Wl