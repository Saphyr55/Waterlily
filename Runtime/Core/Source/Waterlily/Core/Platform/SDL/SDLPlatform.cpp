#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Platform/DefaultDisplayEventHandler.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/Platform.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"
#include "Waterlily/Core/Platform/SDL/SDLDisplay.hpp"

namespace Wl
{

    static SDLDisplay g_display = []
    {
        SDLDisplay display;
        display.SetEventHandler(Wl::MakeShared<DefaultDisplayEventHandler>());
        return display;
    }();

    void PlatformStartup()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            WL_LOG_ERROR("Platform", "Failed to initialize SDL: %s", SDL_GetError());
            return;
        }

        PlatformTimeInit();

        WL_LOG_INFO("Platform", "SDL initialized successfully.");

        DisplayProvider::SetDisplay(&g_display);
    }

    void PlatformShutdown()
    {
        Display* display = DisplayProvider::GetDisplay();

        if (display)
        {
            WL_LOG_INFO("Platform", "Cleaning up display resources.");
            DisplayProvider::SetDisplay(nullptr);
        }

        SDL_Quit();

        WL_LOG_INFO("Platform", "SDL cleaned up successfully.");
    }

}// namespace Wl