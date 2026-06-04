#ifdef _WIN32

#include "Waterlily/Core/Platform/PlatformTime.hpp"

#include <Windows.h>

namespace Wl
{

    void PlatformTimeInit()
    {
    }

    void PlatformDelay(double seconds)
    {
        DWORD milliseconds = static_cast<DWORD>(seconds * 1000.0);
        ::Sleep(milliseconds);
    }

    uint64_t PlatformGetTicks()
    {
        return static_cast<uint64_t>(GetTickCount64());
    }

    uint64_t PlatformGetPerformanceCounter()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return static_cast<uint64_t>(counter.QuadPart);
    }

    uint64_t PlatformGetPerformanceFrequency()
    {
        static LARGE_INTEGER frequency = {0};
        if (frequency.QuadPart == 0)
        {
            QueryPerformanceFrequency(&frequency);
        }
        return static_cast<size_t>(frequency.QuadPart);
    }

    double PlatformGetTime()
    {
        return static_cast<double>(PlatformGetTicks()) / 1000.0;
    }

    double PlatformGetHighResolutionTime()
    {
        LARGE_INTEGER counter, frequency;
        QueryPerformanceCounter(&counter);
        QueryPerformanceFrequency(&frequency);
        return static_cast<double>(counter.QuadPart) / static_cast<double>(frequency.QuadPart);
    }

    double PlatformComputeDeltaTime(uint64_t previousCounter, uint64_t currentCounter)
    {
        size_t frequency = PlatformGetPerformanceFrequency();
        if (frequency == 0)
        {
            return 0.0;
        }

        return static_cast<double>(currentCounter - previousCounter) / static_cast<double>(frequency);
    }

}// namespace Wl

#endif