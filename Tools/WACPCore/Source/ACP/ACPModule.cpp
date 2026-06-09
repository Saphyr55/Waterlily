#include "ACP/ACPModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{
    WL_REGISTER_MODULE(ACPModule, "WACP.Core");

    void ACPModule::OnLoad()
    {
        WL_LOG_INFO("[ACPModule]", "ACP module loaded.");
    }

    void ACPModule::OnStartup()
    {
        WL_LOG_INFO("[ACPModule]", "ACP module started.");
    }

    void ACPModule::OnShutdown()
    {
        WL_LOG_INFO("[ACPModule]", "ACP module stopped.");
    }

    void ACPModule::OnUnload()
    {
        WL_LOG_INFO("[ACPModule]", "ACP module unloaded.");
    }

}// namespace Wl
