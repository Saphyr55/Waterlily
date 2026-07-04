#pragma once

#include "Waterlily/Engine/EngineExports.hpp"

namespace Wl
{

    class ApplicationDelegate;

    class WL_ENGINE_API Application
    {
    public:
        void RequestStop();
        void Pause();
        void Unpause();
        bool IsPaused()
        {
            return m_paused;
        }

        void Start();
        void Stop();

        void Run();

        inline bool IsRunning()
        {
            return m_isRunning;
        }

        explicit Application(ApplicationDelegate* delegate)
            : m_delegate(delegate)
            , m_isRunning(false)
        {
        }

    private:
        ApplicationDelegate* m_delegate;
        bool m_isRunning = false;
        bool m_paused = false;
    };

}// namespace Wl
