#include "Waterlily/Engine/engineModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(EngineModule, "Waterlily.Engine");

    void EngineModule::OnLoad()
    {
        WL_LOG_INFO("[EngineModule]", "Engine module loaded.");
    }

    void EngineModule::OnStartup()
    {
        WL_LOG_INFO("[EngineModule]", "Engine module started.");
    }

    void EngineModule::OnShutdown()
    {
        WL_LOG_INFO("[EngineModule]", "Engine module stopped.");
    }

    void EngineModule::OnUnload()
    {
        WL_LOG_INFO("[EngineModule]", "Engine module unloaded.");
    }

}// namespace Wl