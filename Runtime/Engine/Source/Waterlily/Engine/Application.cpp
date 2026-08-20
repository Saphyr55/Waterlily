#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/Engine/ApplicationDelegate.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"

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

            if (m_delegate && !IsPaused())
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
        Unpause();
        if (m_delegate)
        {
            m_delegate->OnStartup();
        }
    }

    void Application::Pause()
    {
        m_paused = true;
    }

    void Application::Unpause()
    {
        m_paused = false;
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