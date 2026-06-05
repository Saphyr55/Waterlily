#include "Waterlily/Core/Platform/Window.hpp"

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DefaultDisplayEventHandler.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"

namespace Wl
{

    SharedPtr<Window> Window::Create(const WindowProperties& properties, Display& display)
    {
        return MakeShared<Window>(properties, display);
    }

    Window::Window(const WindowProperties& properties, Display& display)
        : m_display(display)
        , m_properties(properties)
        , m_handle(Display::InvalidWindowHandle)
    {
        m_handle = display.CreateWindowHandle(properties);

        SetupEventOnResized();
        SetupDisplayEvents();
    }

    Window::~Window()
    {
    }

    void Window::SetupEventOnResized()
    {
        m_eventHandler.OnResized.Connect([&](uint32_t width, uint32_t height)
        {
            while (width == 0 || height == 0)
            {
                width = GetProperties().Width;
                height = GetProperties().Height;
                Display::GetDefault().WaitEvent();
            }
        });
    }

    void Window::SetupDisplayEvents()
    {
        DisplaySignals::OnWindowClose.Connect([&](WindowHandle handle)
        {
            if (m_handle == handle)
            {
                m_eventHandler.OnClose.Emit();
            }
        });

        DisplaySignals::OnWindowMinimized.Connect([&](WindowHandle handle)
        {
            if (m_handle == handle)
            {
                m_eventHandler.OnMinimized.Emit();
            }
        });

        DisplaySignals::OnWindowShown.Connect([&](WindowHandle handle)
        {
            if (m_handle == handle)
            {
                m_eventHandler.OnShown.Emit();
            }
        });

        DisplaySignals::OnWindowResized.Connect([&](WindowHandle handle, uint32_t width, uint32_t height)
        {
            if (m_handle == handle)
            {
                m_eventHandler.OnResized.Emit(width, height);
            }
        });
    }

    void* Window::GetNativeWindow() const
    {
        return m_display.GetNativeWindowHandle(m_handle);
    }

    void Window::Show()
    {
        m_display.ShowWindow(m_handle);
    }

    void Window::Hide()
    {
        m_display.HideWindow(m_handle);
    }

    void Window::Close()
    {
        m_display.CloseWindow(m_handle);
    }

    const WindowEventHandler& Window::GetEventHandler()
    {
        return m_eventHandler;
    }

    const WindowProperties& Window::GetProperties() const
    {
        return m_properties;
    }

    const WindowProperties& Window::QueryProperties()
    {
        m_properties = m_display.QueryWindowProperties(m_handle);
        return GetProperties();
    }

}// namespace Wl
