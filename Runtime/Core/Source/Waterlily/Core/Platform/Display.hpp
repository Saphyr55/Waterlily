#pragma once

#include "Waterlily/Core/Platform/DisplayEventHandler.hpp"

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"

namespace Wl
{

    class WL_CORE_API Display
    {
    public:
        static constexpr WindowHandle InvalidWindowHandle = UINT32_MAX;
        static constexpr WindowHandle MainWindowHandle = 1;
        static constexpr WindowHandle DesktopWindowHandle = 0;

    public:
        static Display& GetDefault();

        virtual SharedPtr<DisplayEventHandler> GetEventHandler() = 0;

        virtual void SetEventHandler(SharedPtr<DisplayEventHandler> eventHandler) = 0;

        virtual WindowHandle CreateWindowHandle(const WindowProperties& properties) = 0;

        virtual WindowProperties GetWindowProperties(WindowHandle window) = 0;

        virtual WindowProperties QueryWindowProperties(WindowHandle window) = 0;

        virtual void ShowWindow(WindowHandle window) = 0;

        virtual void HideWindow(WindowHandle window) = 0;

        virtual void CloseWindow(WindowHandle window) = 0;

        virtual bool IsValid(WindowHandle window) = 0;

        virtual void WaitEvent() = 0;

        virtual void HandleEvents() = 0;

        virtual void* GetNativeWindowHandle(WindowHandle window) = 0;

        virtual ~Display() = default;
    };

    class WL_CORE_API DisplayProvider
    {
    public:
        static Display* GetDisplay();

        static void SetDisplay(Display* display);

    private:
        static Display* s_diplay;
    };

}// namespace Wl
