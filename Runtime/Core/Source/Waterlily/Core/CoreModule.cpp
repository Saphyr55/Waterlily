#include "Waterlily/Core/coreModule.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(CoreModule, "Waterlily.Core");

    void CoreModule::OnLoad()
    {
        WL_LOG_INFO("[CoreModule]", "Core module loaded.");
    }

    void CoreModule::OnStartup()
    {
        WL_LOG_INFO("[CoreModule]", "Core module started.");
    }

    void CoreModule::OnShutdown()
    {
        WL_LOG_INFO("[CoreModule]", "Core module stopped.");
    }

    void CoreModule::OnUnload()
    {
        WL_LOG_INFO("[CoreModule]", "Core module unloaded.");
    }

}// namespace Wl