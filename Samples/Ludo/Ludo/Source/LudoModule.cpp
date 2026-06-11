#include "LudoModule.hpp"

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/RHI/RHIModule.hpp"

WL_REGISTER_MODULE(LudoModule, "Ludo");

void LudoModule::OnLoad()
{
    WL_LOG_INFO("LudoModule", "Ludo module loaded.");
}

void LudoModule::OnStartup()
{
    WL_LOG_INFO("LudoModule", "Starting Ludo application.");
}

void LudoModule::OnShutdown()
{
    WL_LOG_INFO("LudoModule", "Shutting down Ludo application.");
}

void LudoModule::OnUnload()
{
    WL_LOG_INFO("LudoModule", "Ludo module unloaded.");
}
