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
        LinearAllocator allocator(MemoryStack::GetCurrentAllocator(), 32 * WL_MB);

        double lastTime = PlatformGetHighResolutionTime();

        while (IsRunning())
        {
            MemoryScope memoryScope(&allocator);
            allocator.Reset();
            
            Display::GetDefault().HandleEvents();

            double nowTime = PlatformGetHighResolutionTime();
            double deltaTime = nowTime - lastTime;
            lastTime = nowTime;

            if (m_delegate)
            {
                m_delegate->OnUpdate(deltaTime);
                if (!IsPaused())
                {                       
                    m_delegate->OnRender();
                }
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