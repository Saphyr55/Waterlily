#include "Waterlily/Core/coreModule.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(CoreModule, "Waterlily.Core");

    void CoreModule::OnStartup()
    {
        WL_LOG_INFO("CoreModule", "Core module started.");
    }

    void CoreModule::OnShutdown()
    {
        WL_LOG_INFO("CoreModule", "Core module stopped.");
    }

}// namespace Wl