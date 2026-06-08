#include "Waterlily/Core/Platform/SDL/SDLDisplay.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Platform/SDL/SDLInput.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_Events.h>

namespace Wl
{

    void SDLDisplay::SetEventHandler(SharedPtr<DisplayEventHandler> eventHandler)
    {
        m_eventHandler = eventHandler;
    }

    SharedPtr<DisplayEventHandler> SDLDisplay::GetEventHandler()
    {
        return m_eventHandler;
    }

    WindowHandle SDLDisplay::CreateWindowHandle(const WindowProperties& windowProperties)
    {
        WindowHandle handle = m_windowMap.GetSize() + 1;
        m_nativeWindowMap[handle] = nullptr;

        SDL_PropertiesID properties = SDL_CreateProperties();

#if defined(_WIN32)
        SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, m_nativeWindowMap[handle]);
#endif

        SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, windowProperties.Title.data());
        SDL_SetFloatProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowProperties.Width);
        SDL_SetFloatProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowProperties.Height);
        SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
        SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);

        SDL_Window* sdlWindow = SDL_CreateWindowWithProperties(properties);

        if (!sdlWindow)
        {
            WL_LOG_ERROR("[SDLDisplay]", Wl::Format("Failed to create window: %s", SDL_GetError()));
            return Display::InvalidWindowHandle;
        }

        m_windowMap[handle] = sdlWindow;
        m_windowPropertiesMap[handle] = windowProperties;
        m_ReverseWindowMap[sdlWindow] = handle;

        return handle;
    }

    WindowProperties SDLDisplay::GetWindowProperties(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        WindowProperties properties = m_windowPropertiesMap[window];

        WL_LOG_INFO("[SDLDisplay]", Wl::Format("Retrieved window properties for handle: %d", window));

        return properties;
    }

    WindowProperties SDLDisplay::QueryWindowProperties(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        SDL_Window* sdlWindow = m_windowMap[window];

        WL_CHECK(sdlWindow);

        WindowProperties& properties = m_windowPropertiesMap[window];

        int32_t width = 0;
        int32_t height = 0;
        SDL_GetWindowSize(sdlWindow, &width, &height);
        properties.Width = static_cast<float>(width);
        properties.Height = static_cast<float>(height);

        int32_t x = 0;
        int32_t y = 0;
        SDL_GetWindowPosition(sdlWindow, &x, &y);
        properties.X = static_cast<float>(x);
        properties.Y = static_cast<float>(y);

        properties.Title = SDL_GetWindowTitle(sdlWindow);

        return properties;
    }

    void SDLDisplay::ShowWindow(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        SDL_Window* sdlWindow = m_windowMap[window];

        WL_CHECK(sdlWindow);

        SDL_ShowWindow(sdlWindow);

        WL_LOG_INFO("[SDLDisplay]", Wl::Format("Showing window with handle: %d", window));
    }

    void SDLDisplay::HideWindow(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        SDL_Window* sdlWindow = m_windowMap[window];

        WL_CHECK(sdlWindow);

        SDL_HideWindow(sdlWindow);
    }

    void SDLDisplay::CloseWindow(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        SDL_Window* sdlWindow = m_windowMap[window];
        WL_CHECK(sdlWindow);

        SDL_PropertiesID propertiesID = SDL_GetWindowProperties(sdlWindow);

        m_ReverseWindowMap.Remove(sdlWindow);
        m_windowMap.Remove(window);

        SDL_DestroyProperties(propertiesID);
        SDL_DestroyWindow(sdlWindow);
    }

    bool SDLDisplay::IsValid(WindowHandle window)
    {
        if (window == Display::InvalidWindowHandle)
        {
            return false;
        }

        return m_windowMap.Contains(window) && m_windowMap[window] != nullptr;
    }

    void SDLDisplay::WaitEvent()
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
    }

    void SDLDisplay::HandleEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (!m_eventHandler)
            {
                break;
            }

            switch (event.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                    SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);
                    WindowHandle handle = m_ReverseWindowMap[window];
                    m_eventHandler->OnWindowClose(handle);
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);
                    WindowHandle handle = m_ReverseWindowMap[window];
                    m_eventHandler->OnWindowResized(handle, event.window.data1, event.window.data2);
                    break;
                }
                case SDL_EVENT_WINDOW_SHOWN: {
                    SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);
                    WindowHandle handle = m_ReverseWindowMap[window];
                    m_eventHandler->OnWindowShown(handle);
                    break;
                }
                case SDL_EVENT_WINDOW_MINIMIZED: {
                    SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);
                    WindowHandle handle = m_ReverseWindowMap[window];
                    m_eventHandler->OnWindowMinimized(handle);
                    break;
                }
                case SDL_EVENT_MOUSE_WHEEL: {
                    m_eventHandler->OnMouseWheel(event.wheel.mouse_x);
                    break;
                }
                case SDL_EVENT_KEY_UP: {
                    m_eventHandler->OnKeyUp(KeyFromSDL(event.key.key));
                    break;
                }
                case SDL_EVENT_KEY_DOWN: {
                    m_eventHandler->OnKeyDown(KeyFromSDL(event.key.key));
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    m_eventHandler->OnButtonUp(ButtonFromSDL(event.button.button));
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    m_eventHandler->OnButtonDown(ButtonFromSDL(event.button.button));
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION: {
                    m_eventHandler->OnMouseMove(event.motion.xrel, event.motion.yrel, event.motion.x, event.motion.y);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void* SDLDisplay::GetNativeWindowHandle(WindowHandle window)
    {
        WL_CHECK_MSG(IsValid(window), "Invalid window handle provided.");

        SDL_Window* sdlWindow = m_windowMap[window];
        WL_CHECK(sdlWindow);

#ifdef _WIN32
        SDL_PropertiesID propertiesID = SDL_GetWindowProperties(sdlWindow);
        void* hwnd = SDL_GetPointerProperty(propertiesID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        WL_CHECK(hwnd);
        m_nativeWindowMap[window] = hwnd;
        return hwnd;
#endif

        return static_cast<void*>(sdlWindow);
    }

}// namespace Wl