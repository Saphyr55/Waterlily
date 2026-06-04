#include "Waterlily/Entity/EntityModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(EntityModule, "Waterlily.Entity");

    void EntityModule::OnLoad()
    {
        LLOG_INFO("[EntityModule]", "Entity module loaded.");
    }

    void EntityModule::OnStartup()
    {
        LLOG_INFO("[SchemaModule]", "Entity module started.");
    }

    void EntityModule::OnShutdown()
    {
        LLOG_INFO("[EntityModule]", "Entity module stopped.");
    }

    void EntityModule::OnUnload()
    {
        LLOG_INFO("[EntityModule]", "Entity module unloaded.");
    }

}// namespace Wl