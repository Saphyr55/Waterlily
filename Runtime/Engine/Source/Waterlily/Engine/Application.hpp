#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Engine/EngineExports.hpp"

namespace Wl
{

    class ApplicationDelegate;

    class WL_ENGINE_API Application
    {
    public:
        void Run();

        void RequestStop();

        void Start();
        void Stop();

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
    };

}// namespace Wl
