#include "Waterlily/Messaging/MessageModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(MessagingModule, "Waterlily.Messaging");

    void MessagingModule::OnLoad()
    {
        LLOG_INFO("[MessagingModule]", "Messaging module loaded.");
    }

    void MessagingModule::OnStartup()
    {
        LLOG_INFO("[MessagingModule]", "Messaging module started.");
    }

    void MessagingModule::OnShutdown()
    {
        LLOG_INFO("[MessagingModule]", "Messaging module stopped.");
    }

    void MessagingModule::OnUnload()
    {
        LLOG_INFO("[MessagingModule]", "Messaging module unloaded.");
    }

}// namespace Wl