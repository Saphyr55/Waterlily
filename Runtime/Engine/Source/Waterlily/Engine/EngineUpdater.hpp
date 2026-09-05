#pragma once

namespace Wl
{

    class EngineUpdater
    {
    public:
        virtual void OnStartup() = 0;

        virtual void OnShutdown() = 0;

        virtual void OnTick(double deltaTime) = 0;

        virtual ~EngineUpdater() = default;
    };

}// namespace Wl