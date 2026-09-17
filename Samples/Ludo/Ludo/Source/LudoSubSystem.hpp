#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Engine/EngineSubSystem.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderService.hpp"
#include "Waterlily/Scene/Camera.hpp"

using namespace Wl;

namespace Ludo
{

    inline const StringID LudoSubSystemName = WL_SID("LudoSubSystem");

    class LUDO_API LudoSubSystem : public EngineSubSystem
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnTick(double deltaTime) override;

        static inline Camera CreateCamera()
        {
            return Camera::Create(Vector3f(-6.0f, 1.0f, -0.1f), Vector3f(-15.0f, 1.0f, 0.0f), 6.0f);
        }

        LudoSubSystem(const SharedPtr<RenderService>& renderService,
                      const SharedPtr<AssetManager>& assetManager)
            : m_renderService(renderService)
            , m_assetManager(assetManager)
        {
        }
        virtual ~LudoSubSystem() = default;

    private:
        SharedPtr<RenderService> m_renderService;
        SharedPtr<AssetManager> m_assetManager;

        // Scene Data.
        EntityRegistry m_entityRegistry;
        Camera m_camera;

        // Render Data.
        SharedPtr<RenderMesh> m_sponzaMesh = nullptr;
        RHIBuffer* m_indirectBuffer = nullptr;
        size_t m_indirectBufferCount = 0;
    };

}// namespace Wl
