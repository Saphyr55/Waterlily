#include "LudoModule.hpp"

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/RHI/RHIModule.hpp"

WL_REGISTER_MODULE(LudoModule, "Ludo");

void LudoModule::OnLoad()
{
    WL_LOG_INFO("[LudoModule]", "Ludo module loaded.");
}

void LudoModule::OnStartup()
{
    // Create a window. WindowHandle behave like a reference.
    WindowProperties windowProperties("Demo Window", 1080, 720, 100, 100);
    m_window = Window::Create(windowProperties);

    // Module Registry manages the module.
    ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();
    RHIModule* rhiModule = moduleRegistry.GetModule<RHIModule>("Waterlily.RHI");

    // Need to know the window for creating the surface.
    rhiModule->SetNativeWindow(m_window->GetNativeWindow());

    // Vulkan API is already the default option.
    rhiModule->SetGraphicsAPI(GraphicsAPI::Vulkan);

    WL_LOG_INFO("[LudoModule]", "Starting Ludo application.");
}

void LudoModule::OnShutdown()
{
    WL_LOG_INFO("[LudoModule]", "Shutting down Ludo application.");
}

void LudoModule::OnUnload()
{
    WL_LOG_INFO("[LudoModule]", "Ludo module unloaded.");
}
