#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/EngineSubSystem.hpp"
#include "Waterlily/Renderer/RenderService.hpp"

using namespace Wl;

namespace Ludo
{

    inline const StringID LudoDevSubSystemName = StringID("LudoDevSubSystem");

    class LudoDevSubSystem : public EngineSubSystem
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnTick(double deltaTime) override;

    public:
        LudoDevSubSystem(SharedPtr<RenderService> renderService)
            : m_renderService(renderService)
        {
        }
        virtual ~LudoDevSubSystem() override = default;
    
    private:
        SharedPtr<RenderService> m_renderService;
    };

}// namespace Ludo