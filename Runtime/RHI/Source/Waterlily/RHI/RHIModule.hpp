#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/RHIExports.hpp"

namespace Wl
{

    enum class GraphicsAPI
    {
        Vulkan,
    };

    StringRef GetGraphicsAPIName(GraphicsAPI graphics_api);

    class WL_RHI_API RHIModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

        inline void SetWindowHandle(WindowHandle window_handle)
        {
            m_windowHandle = window_handle;
        }

        inline WindowHandle GetWindowHandle()
        {
            return m_windowHandle;
        }

        inline void SetDevice(SharedPtr<RHIDevice> device)
        {
            m_device = device;
        }

        inline SharedPtr<RHIDevice> GetDevice()
        {
            return m_device;
        }

        inline GraphicsAPI GetGraphicsAPI()
        {
            return m_graphicsAPI;
        }

        inline void SetGraphicsAPI(GraphicsAPI graphicsAPI)
        {
            m_graphicsAPI = graphicsAPI;
        }

        RHIModule();

    private:
        WindowHandle m_windowHandle;
        GraphicsAPI m_graphicsAPI;
        SharedPtr<RHIDevice> m_device;
    };

}// namespace Wl