#pragma once

#include "Timer.hpp"
#include "Waterlily/Core/Signals/Signal.hpp"
#include "Waterlily/Engine/EngineExports.hpp"
#include "Waterlily/Engine/Timer.hpp"

namespace Wl
{

    class WL_ENGINE_API Application
    {
    public:
        inline constexpr static double DefaultTargetFPS = 144.0;
        inline constexpr static double DefaultTargetFrameRate = 1000.0 / DefaultTargetFPS;
        inline constexpr static double UnlimtedFrameRate = 0.0;

    public:
        WL_SIGNAL(OnTick, Timer& /* timer */);

    public:
        void Start();
        void Stop();
        void Run();

        void ToogleUnlimitedFrameRate(double targetFrameRate);

        inline bool IsRunning()
        {
            return m_isRunning;
        }

        const Timer& GetTimer()
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
        Timer m_timer;
        double m_targetFrameRate = DefaultTargetFrameRate;
        bool m_isRunning;
    };

}// namespace Wl
