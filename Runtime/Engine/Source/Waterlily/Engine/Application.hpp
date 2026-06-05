#pragma once

#include "Waterlily/Core/Signals/Signal.hpp"
#include "Waterlily/Core/Time/DeltaTimer.hpp"
#include "Waterlily/Engine/engineExports.hpp"

namespace Wl
{

    class WL_ENGINE_API Application
    {
    public:
        inline constexpr static double DefaultTargetFPS = 144.0;
        inline constexpr static double DefaultTargetFrameRate = 1.0 / DefaultTargetFPS;
        inline constexpr static double UnlimtedFrameRate = 0.0;

    public:
        WL_SIGNAL(OnTick, double /* deltaTime */);

    public:
        void Start();
        void Stop();
        void Run();

        void ToogleUnlimitedFrameRate(double target_frame_rate);

        inline bool IsRunning()
        {
            return m_isRunning;
        }

        const DeltaTimer& GetTimer()
        {
            return m_timer;
        }

        inline double GetTargetFrameRate()
        {
            return m_targetFrameRate;
        }

        inline bool IsUnlimitedFrameRate()
        {
            return m_targetFrameRate == UnlimtedFrameRate;
        }

        inline void SetTargetFrameRate(double targetFrameRate)
        {
            m_targetFrameRate = targetFrameRate;
        }

    private:
        DeltaTimer m_timer;
        double m_targetFrameRate = DefaultTargetFrameRate;
        bool m_isRunning;
    };

}// namespace Wl
