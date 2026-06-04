#include "Waterlily/Core/Time/DeltaTimer.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

namespace Wl
{

    double DeltaTimer::Tick()
    {
        size_t currentCounter = PlatformGetPerformanceCounter();

        if (m_isFirstCall)
        {
            m_lastCounter = currentCounter;
            m_isFirstCall = false;
            m_deltaTime = 0.0;
            return 0.0;
        }

        m_deltaTime = PlatformComputeDeltaTime(m_lastCounter, currentCounter);
        m_lastCounter = currentCounter;

        return m_deltaTime * m_timeScale;
    }

    double DeltaTimer::LimitFrameRate(double deltaTime, double targetFrameRate)
    {
        if (deltaTime < targetFrameRate)
        {
            double sleepTime = targetFrameRate - deltaTime;
            PlatformDelay(sleepTime);
            return Tick();
        }
        return deltaTime;
    }

}// namespace Wl