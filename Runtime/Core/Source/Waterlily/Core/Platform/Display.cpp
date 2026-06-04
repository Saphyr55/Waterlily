#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DefaultDisplayEventHandler.hpp"

namespace Wl
{

    Display* DisplayProvider::s_diplay = nullptr;

    Display* DisplayProvider::GetDisplay()
    {
        return s_diplay;
    }

    void DisplayProvider::SetDisplay(Display* display)
    {
        s_diplay = display;
    }

    Display& Display::GetDefault()
    {
        Display* display = DisplayProvider::GetDisplay();
        WL_CHECK_MSG(display, "Display is not initialized.");
        return *display;
    }

}// namespace Wl