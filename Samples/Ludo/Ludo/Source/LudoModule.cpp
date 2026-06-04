#include "LudoModule.hpp"

#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/RHI/RHIModule.hpp"

WL_REGISTER_MODULE(LudoModule, "Ludo");

void LudoModule::OnLoad()
{
    // Create a window. WindowHandle behave like a reference.
    Display& display = Display::GetDefault();
    WindowProperties window_properties("Demo Window", 1080, 720, 100, 100);
    WindowHandle window_handle = display.CreateWindowHandle(window_properties);

    // Module Registry manages the module.
    ModuleRegistry& module_registry = ModuleRegistry::GetInstance();
    RHIModule* RHI_module = module_registry.GetModule<RHIModule>("Waterlily.RHI");

    // Need to know the window for creating the surface.
    RHI_module->SetWindowHandle(window_handle);

    // Vulkan API is already the default option.
    RHI_module->SetGraphicsAPI(GraphicsAPI::Vulkan);

    LLOG_INFO("[LudoModule]", "Ludo module loaded.");
}

void LudoModule::OnStartup()
{
    LLOG_INFO("[LudoModule]", "Starting Ludo application.");
}

void LudoModule::OnShutdown()
{
    LLOG_INFO("[LudoModule]", "Shutting down Ludo application.");
}

void LudoModule::OnUnload()
{
    LLOG_INFO("[LudoModule]", "Ludo module unloaded.");
}
