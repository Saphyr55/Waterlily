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
        static const Uint64 s_frequency = SDL_GetPerformanceFrequency();
        const Uint64 counter = SDL_GetPerformanceCounter();

        return static_cast<double>(counter) / static_cast<double>(s_frequency);
    }

    void PlatformDelay(double seconds)
    {
        if (seconds <= 0.0)
        {
            return;
        }

        SDL_Delay(static_cast<Uint32>(seconds * 1000.0));
    }

}// namespace Wl
