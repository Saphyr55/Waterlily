#pragma once

#include "Waterlily/Engine/EngineExports.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

namespace Wl
{

    class WL_ENGINE_API Timer
    {
    public:
        void Tick();

        void LimitFrameRate(double targetFrameRate);

        inline double GetTime()
        {
            return PlatformGetHighResolutionTime();
        }

        inline double GetDeltaTime() const
        {
            return m_deltaTime;
        }

        inline double GetDeltaTimeSeconds() const
        {
            return m_deltaTime * 0.001;
        }

    public:
        Timer();
        ~Timer() = default;

    private:
        double m_deltaTime = 0.0;
        double m_lastTime = 0.0;
    };

}// namespace Wl