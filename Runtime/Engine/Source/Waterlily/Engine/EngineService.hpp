#pragma once

namespace Wl 
{
    
    class EngineService
    {
    public:
        virtual void OnStartup() = 0;

        virtual void OnShutdown() = 0;

        virtual ~EngineService() = default;
    };

}