#include "Waterlily/Engine/Application.hpp"
#include "ApplicationDelegate.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

namespace Wl
{

    void Application::Run()
    {
        double lastTime = PlatformGetHighResolutionTime();

        while (IsRunning())
        {
            Display::GetDefault().HandleEvents();
            
            double nowTime = PlatformGetHighResolutionTime();
            double deltaTime = nowTime - lastTime;
            lastTime = nowTime;

            if (m_delegate)
            {
                m_delegate->OnUpdate(deltaTime);
                m_delegate->OnRender();
            }
        }
    }

    void Application::RequestStop()
    {
        m_isRunning = false;
    }

    void Application::Start()
    {
        m_isRunning = true;
        if (m_delegate)
        {
            m_delegate->OnStartup();
        }
    }

    void Application::Stop()
    {
        if (m_delegate)
        {
            m_delegate->OnShutdown();
        }
        m_isRunning = false;
    }

}// namespace Wl