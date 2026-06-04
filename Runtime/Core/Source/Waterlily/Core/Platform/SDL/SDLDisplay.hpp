#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"

namespace Wl
{

    class SDLDisplay : public Display
    {
    public:
        virtual SharedPtr<DisplayEventHandler> GetEventHandler() override;

        virtual void SetEventHandler(SharedPtr<DisplayEventHandler> eventHandler) override;

        virtual WindowHandle CreateWindowHandle(const WindowProperties& properties) override;

        virtual WindowProperties GetWindowProperties(WindowHandle window) override;

        virtual WindowProperties QueryWindowProperties(WindowHandle window) override;

        virtual void ShowWindow(WindowHandle window) override;

        virtual void HideWindow(WindowHandle window) override;

        virtual void CloseWindow(WindowHandle window) override;

        virtual bool IsValid(WindowHandle window) override;

        virtual void WaitEvent() override;

        virtual void HandleEvents() override;

        virtual void* GetNativeWindowHandle(WindowHandle window) override;

        SDLDisplay() = default;
        ~SDLDisplay() = default;

    private:
        SharedPtr<DisplayEventHandler> m_eventHandler;
        HashMap<WindowHandle, void*> m_nativeWindowMap;
        HashMap<WindowHandle, SDL_Window*> m_windowMap;
        HashMap<SDL_Window*, WindowHandle> m_ReverseWindowMap;
        HashMap<WindowHandle, WindowProperties> m_windowPropertiesMap;
    };

}// namespace Wl