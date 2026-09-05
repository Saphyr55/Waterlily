#pragma once

#include "Waterlily/Engine/EngineUpdater.hpp"
#include "Waterlily/Renderer/RenderService.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    class WL_RENDERER_API RenderUpdater : public EngineUpdater
    {
    public:
        virtual void OnStartup() override;
        
        virtual void OnShutdown() override;

        virtual void OnTick(double deltaTime) override;

    public:
        RenderUpdater(RenderService& m_renderService);

    private:
        RenderService& m_renderService;
    };

}// namespace Wl
