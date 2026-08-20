#include "Waterlily/Engine/engineModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(EngineModule, "Waterlily.Engine");

    void EngineModule::OnStartup()
    {
        WL_LOG_INFO("EngineModule", "Engine module started.");
    }

    void EngineModule::OnShutdown()
    {
        WL_LOG_INFO("EngineModule", "Engine module stopped.");
    }

}// namespace Wl