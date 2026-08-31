#include "ACP/ACPModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{
    WL_REGISTER_MODULE(ACPModule, "WACP.Core");

    void ACPModule::OnStartup()
    {
        WL_LOG_INFO("ACPModule", "ACP module started.");
    }

    void ACPModule::OnShutdown()
    {
        WL_LOG_INFO("ACPModule", "ACP module stopped.");
    }

}// namespace Wl
