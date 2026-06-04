#include "Waterlily/Core/Platform/DefaultDisplayEventHandler.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/Platform.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"
#include "Waterlily/Core/Platform/SDL/SDLDisplay.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    static SDLDisplay g_display = []
    {
        SDLDisplay display;
        display.SetEventHandler(MakeShared<DefaultDisplayEventHandler>());
        return display;
    }();

    void PlatformStartup()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            LLOG_ERROR("[Platform]", Wl::Format("Failed to initialize SDL: %s", SDL_GetError()));
            return;
        }

        PlatformTimeInit();

        LLOG_INFO("[Platform]", "SDL initialized successfully.");

        DisplayProvider::SetDisplay(&g_display);
    }

    void PlatformShutdown()
    {
        Display* display = DisplayProvider::GetDisplay();

        if (display)
        {
            LLOG_INFO("[Platform]", "Cleaning up display resources.");
            DisplayProvider::SetDisplay(nullptr);
        }

        SDL_Quit();

        LLOG_INFO("[Platform]", "SDL cleaned up successfully.");
    }

}// namespace Wl