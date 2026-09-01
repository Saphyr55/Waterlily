#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Proxies/RenderInstance.hpp"
#include "Waterlily/Renderer/Proxies/RenderLight.hpp"
#include "Waterlily/Renderer/Proxies/RenderView.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"

namespace Wl
{

    class Frame;
    class RenderService;
    class Model;

    class RenderScene
    {
    public:
        // TODO: Must pass the scene.
        void Activate(SharedPtr<AssetManager> assetManager, ArrayView<Model*> models);

        void Desactivate();

        void PrepareFrame(EntityRegistry& registry, Frame& frame, const Camera& camera);

    public:
        ArrayView<RenderInstance> GetInstances() const
        {
            return m_instances;
        }

        ArrayView<RenderLight> GetLights() const
        {
            return m_lights;
        }

        const RenderDirectionalLight& GetDirectionalLight() const
        {
            return m_directionalLight;
        }

        const RenderView& GetView() const
        {
            return m_view;
        }

        RHIBuffer* GetIndirectBuffer() const
        {
            return m_indirectBuffer;
        }

        uint32_t GetDrawCount() const
        {
            return m_indirectBufferCount;
        }

    public:
        RenderScene(const SharedPtr<RenderService>& renderService);
        ~RenderScene() = default;

    private:
        SharedPtr<RenderService> m_renderService;

        // Scene Mesh.
        RenderMesh m_mesh;

        // Indirect draw calls.
        RHIBuffer* m_indirectBuffer = nullptr;
        size_t m_indirectBufferCount = 0;

        // Instances.
        Array<RenderInstance> m_instances;
        RenderAllocation m_instanceAllocation;

        // Lights.
        Array<RenderLight> m_lights;
        RenderAllocation m_pointLightsAllocation;

        // Directional Light
        RenderDirectionalLight m_directionalLight;
        RenderAllocation m_directionalLightAllocation;

        // View
        RenderView m_view;
        RenderAllocation m_viewAllocation;

        // Counters
        RenderAllocation m_countersAllocation;
    };

}// namespace Wl