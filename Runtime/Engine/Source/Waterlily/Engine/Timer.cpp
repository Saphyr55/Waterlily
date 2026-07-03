#include "Waterlily/Engine/Timer.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

namespace Wl
{

    Timer::Timer()
    {
    }

    void Timer::Tick()
    {
        double now = GetTime();
        m_deltaTime = now - m_lastTime;
        m_lastTime = now;
    }

    void Timer::LimitFrameRate(double targetFrameTime)
    {
        double now = GetTime();
        double elapsed = now - m_lastTime;
        double remaining = targetFrameTime - elapsed;
        if (remaining > 0.0)
        {
            Wl::PlatformDelay(remaining);
        }
    }

}// namespace Wl