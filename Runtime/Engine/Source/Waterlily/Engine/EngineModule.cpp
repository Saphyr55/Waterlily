#include "Waterlily/Engine/engineModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(EngineModule, "Waterlily.Engine");

    void EngineModule::OnLoad()
    {
        LLOG_INFO("[EngineModule]", "Engine module loaded.");
    }

    void EngineModule::OnStartup()
    {
        LLOG_INFO("[EngineModule]", "Engine module started.");
    }

    void EngineModule::OnShutdown()
    {
        LLOG_INFO("[EngineModule]", "Engine module stopped.");
    }

    void EngineModule::OnUnload()
    {
        LLOG_INFO("[EngineModule]", "Engine module unloaded.");
    }

}// namespace Wl