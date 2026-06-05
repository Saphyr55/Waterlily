#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/Signals/Signal.hpp"

namespace Wl
{

    struct WL_CORE_API WindowEventHandler
    {
        WL_SIGNAL(OnClose);
        WL_SIGNAL(OnResized, uint32_t /* width */, uint32_t /* height */);
        WL_SIGNAL(OnMinimized);
        WL_SIGNAL(OnShown);
    };

    class WL_CORE_API Window
    {
    public:
        static SharedPtr<Window> Create(const WindowProperties& properties, Display& diplay = Display::GetDefault());

        void Show();
        void Hide();
        void Close();

        const WindowEventHandler& GetEventHandler();

        void* GetNativeWindow() const;

        const WindowProperties& GetProperties() const;
        const WindowProperties& QueryProperties();

    public:
        Window(const WindowProperties& properties, Display& display);
        ~Window();

    private:
        void SetupEventOnResized();
        void SetupDisplayEvents();

    private:
        WindowEventHandler m_eventHandler;
        Display& m_display;
        WindowProperties m_properties;
        WindowHandle m_handle;
    };

}// namespace Wl
