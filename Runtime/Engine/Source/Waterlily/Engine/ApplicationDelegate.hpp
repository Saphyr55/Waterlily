#pragma once

namespace Wl
{

    class ApplicationDelegate
    {
    public:
        virtual void OnStartup() = 0;

        virtual void OnShutdown() = 0;

        virtual void OnUpdate(double deltaTime) = 0;

        virtual void OnRender() = 0;
    };

}// namespace Wl