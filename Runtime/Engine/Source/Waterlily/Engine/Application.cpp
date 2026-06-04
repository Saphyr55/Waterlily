#include "Waterlily/Engine/Application.hpp"

namespace Wl
{

    void Application::ToogleUnlimitedFrameRate(double targetFrameRate)
    {
        if (m_targetFrameRate == UnlimtedFrameRate)
        {
            m_targetFrameRate = targetFrameRate;
        }
        else
        {
            m_targetFrameRate = UnlimtedFrameRate;
        }
    }

    void Application::Run()
    {
        while (IsRunning())
        {
            double deltaTime = m_timer.Tick();

            // Frame rate limiting.
            if (!IsUnlimitedFrameRate())
            {
                deltaTime = m_timer.LimitFrameRate(deltaTime, m_targetFrameRate);
            }

            OnTick.Emit(deltaTime);
        }
    }

    void Application::Start()
    {
        m_isRunning = true;
    }

    void Application::Stop()
    {
        m_isRunning = false;
    }

}// namespace Wl