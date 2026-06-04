#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class WL_CORE_API DeltaTimer
    {
    public:
        double Tick();

        double LimitFrameRate(double delta_time, double target_frame_rate);

        inline double GetDeltaTime() const
        {
            return m_deltaTime;
        }

        inline void SetTimeScale(double scale)
        {
            m_timeScale = scale;
        }

        inline double GetTimeScale() const
        {
            return m_timeScale;
        }

        inline void reset()
        {
            m_isFirstCall = true;
            m_deltaTime = 0.0;
        }

    public:
        DeltaTimer() = default;
        ~DeltaTimer() = default;

    private:
        size_t m_lastCounter = 0;
        bool m_isFirstCall = true;
        double m_deltaTime = 0.0;
        double m_timeScale = 1.0f;
    };

}// namespace Wl