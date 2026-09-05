#include "Waterlily/RHI/RHIModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RHIModule, "Waterlily.RHI");

    void RHIModule::OnStartup()
    {
        WL_LOG_INFO("RHIModule", "Start up RHI Module.");
    }

    void RHIModule::OnShutdown()
    {
        WL_LOG_INFO("RHIModule", "Shuting down RHI Module...");
    }

}// namespace Wl