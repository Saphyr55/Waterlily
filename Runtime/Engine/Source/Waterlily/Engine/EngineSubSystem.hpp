#pragma once

namespace Wl
{

    class EngineSubSystem
    {
    public:
        virtual void OnStartup() = 0;

        virtual void OnShutdown() = 0;

        virtual void OnTick(double deltaTime) = 0;

        virtual ~EngineSubSystem() = default;
    };

}// namespace Wl