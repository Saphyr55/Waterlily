#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"

namespace Wl
{

    class WL_CORE_API DisplayEventHandler
    {
    public:
        virtual void OnWindowClose(WindowHandle window)
        {
        }

        virtual void OnWindowResized(WindowHandle window, uint32_t width, uint32_t height)
        {
        }

        virtual void OnWindowMinimized(WindowHandle window)
        {
        }

        virtual void OnWindowShown(WindowHandle window)
        {
        }

        virtual void OnMouseWheel(float delta)
        {
        }

        virtual void OnKeyUp(VirtualKey key)
        {
        }

        virtual void OnKeyDown(VirtualKey key)
        {
        }

        virtual void OnButtonUp(Button button)
        {
        }

        virtual void OnButtonDown(Button button)
        {
        }

        virtual void OnMouseMove(float pos_rel_x, float pos_rel_y, float pos_x, float pos_y)
        {
        }

        virtual ~DisplayEventHandler() = default;
    };

}// namespace Wl