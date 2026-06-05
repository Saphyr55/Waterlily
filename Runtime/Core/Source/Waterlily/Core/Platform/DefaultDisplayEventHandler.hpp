#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Platform/DisplayEventHandler.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/Signals/Signal.hpp"

namespace Wl
{

    struct WL_CORE_API DisplaySignals
    {
        WL_SIGNAL_STATIC(OnWindowClose, WindowHandle /* window*/);
        WL_SIGNAL_STATIC(OnWindowResized, WindowHandle /* window */, uint32_t /* width*/, uint32_t /* height */);
        WL_SIGNAL_STATIC(OnWindowMinimized, WindowHandle /* window */);
        WL_SIGNAL_STATIC(OnWindowShown, WindowHandle /* window */);
    };

    class WL_CORE_API DefaultDisplayEventHandler : public DisplayEventHandler
    {
    public:
        virtual void OnWindowClose(WindowHandle window) override;

        virtual void OnWindowResized(WindowHandle window, uint32_t width, uint32_t height) override;

        virtual void OnWindowMinimized(WindowHandle window) override;

        virtual void OnWindowShown(WindowHandle window) override;

        virtual void OnMouseWheel(float delta) override;

        virtual void OnKeyDown(VirtualKey key) override;

        virtual void OnKeyUp(VirtualKey key) override;

        virtual void OnMouseMove(float posRelX, float posRelY, float posX, float posY) override;

        virtual void OnButtonUp(Button button) override;

        virtual void OnButtonDown(Button button) override;
    };

}// namespace Wl
