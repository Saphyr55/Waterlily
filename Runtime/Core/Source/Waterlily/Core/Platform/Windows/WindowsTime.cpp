#ifdef _WIN32

#include "Waterlily/Core/Platform/PlatformTime.hpp"

#include <Windows.h>

namespace Wl
{
    
    static LARGE_INTEGER s_Frequency;
    static bool s_Initialized = false;

    void PlatformTimeInit()
    {
        QueryPerformanceFrequency(&s_Frequency);
        s_Initialized = true;
    }

    double PlatformGetHighResolutionTime()
    {
        if (!s_Initialized)
        {
            PlatformTimeInit();
        }

        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        return static_cast<double>(counter.QuadPart) * 1000.0 / static_cast<double>(s_Frequency.QuadPart);
    }

    void PlatformDelay(double ms)
    {
        if (ms <= 0.0) 
        {
            return;
        }

        Sleep(static_cast<DWORD>(ms));
    }


}// namespace Wl

#endif