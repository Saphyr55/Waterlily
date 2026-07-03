#include "SDL3/SDL_timer.h"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

#include <SDL3/SDL.h>

namespace Wl
{

    void PlatformTimeInit()
    {
    }

    double PlatformGetHighResolutionTime()
    {
        return SDL_NS_TO_SECONDS(static_cast<double>(SDL_GetTicksNS()));
    }

    void PlatformDelay(double seconds)
    {
        SDL_DelayNS(SDL_SECONDS_TO_NS(seconds));
    }

}// namespace Wl
