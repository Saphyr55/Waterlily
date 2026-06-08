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
            m_timer.Tick();
            OnTick.Emit(m_timer);
             
            if (!IsUnlimitedFrameRate())
            {
                m_timer.LimitFrameRate(m_targetFrameRate);
            }
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