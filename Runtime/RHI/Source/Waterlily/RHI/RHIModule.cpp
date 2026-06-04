#include "Waterlily/RHI/RHIModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RHIModule, "Waterlily.RHI");

    StringRef GetGraphicsAPIName(const GraphicsAPI graphicsAPI)
    {
        switch (graphicsAPI)
        {
            case GraphicsAPI::Vulkan: {
                return "Waterlily.RHI.Vulkan";
            }
        }
        return "Waterlily.RHI.Vulkan";
    }

    RHIModule::RHIModule()
        : m_windowHandle(Display::InvalidWindowHandle)
        , m_graphicsAPI(GraphicsAPI::Vulkan)
    {
    }

    void RHIModule::OnLoad()
    {
        LLOG_INFO("[RHIModule]", "Loading RHI Module.");
    }

    void RHIModule::OnStartup()
    {
        LLOG_INFO("[RHIModule]", "Start up RHI Module.");
    }

    void RHIModule::OnShutdown()
    {
        LLOG_INFO("[RHIModule]", "Shuting down RHI Module...");
    }

    void RHIModule::OnUnload()
    {
        LLOG_INFO("[RHIModule]", "Unload RHI Module.");
    }

}// namespace Wl