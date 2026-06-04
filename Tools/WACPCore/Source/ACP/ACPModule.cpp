#include "ACP/ACPModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{
    WL_REGISTER_MODULE(ACPModule, "WACP.Core");

    void ACPModule::OnLoad()
    {
        LLOG_INFO("[ACPModule]", "ACP module loaded.");
    }

    void ACPModule::OnStartup()
    {
        LLOG_INFO("[ACPModule]", "ACP module started.");
    }

    void ACPModule::OnShutdown()
    {
        LLOG_INFO("[ACPModule]", "ACP module stopped.");
    }

    void ACPModule::OnUnload()
    {
        LLOG_INFO("[ACPModule]", "ACP module unloaded.");
    }

}// namespace Wl
