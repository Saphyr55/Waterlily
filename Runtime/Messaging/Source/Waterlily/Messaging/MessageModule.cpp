#include "Waterlily/Messaging/MessageModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(MessagingModule, "Waterlily.Messaging");

    void MessagingModule::OnLoad()
    {
        WL_LOG_INFO("MessagingModule", "Messaging module loaded.");
    }

    void MessagingModule::OnStartup()
    {
        WL_LOG_INFO("MessagingModule", "Messaging module started.");
    }

    void MessagingModule::OnShutdown()
    {
        WL_LOG_INFO("MessagingModule", "Messaging module stopped.");
    }

    void MessagingModule::OnUnload()
    {
        WL_LOG_INFO("MessagingModule", "Messaging module unloaded.");
    }

}// namespace Wl