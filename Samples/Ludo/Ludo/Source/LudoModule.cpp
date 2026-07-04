#include "LudoModule.hpp"

#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Engine/Engine.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(LudoModule, "Ludo");

    void LudoModule::OnLoad()
    {
        WL_LOG_INFO("LudoModule", "Ludo module loaded.");
    }

    void LudoModule::OnStartup()
    {
        WL_LOG_INFO("LudoModule", "Starting Ludo application.");

        Engine::GetInstance().SetApplicationDelegate(&m_applicationState);
    }

    void LudoModule::OnShutdown()
    {
        Engine::GetInstance().SetApplicationDelegate(nullptr);

        WL_LOG_INFO("LudoModule", "Shutting down Ludo application.");
    }

    void LudoModule::OnUnload()
    {
        WL_LOG_INFO("LudoModule", "Ludo module unloaded.");
    }

}// namespace Wl
