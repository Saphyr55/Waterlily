#include "Waterlily/Entity/EntityModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(EntityModule, "Waterlily.Entity");

    void EntityModule::OnStartup()
    {
        WL_LOG_INFO("EntityModule", "Entity module started.");
    }

    void EntityModule::OnShutdown()
    {
        WL_LOG_INFO("EntityModule", "Entity module stopped.");
    }

}// namespace Wl