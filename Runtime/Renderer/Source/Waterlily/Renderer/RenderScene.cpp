#include "RenderScene.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderService.hpp"

namespace Wl
{
    
    void RenderScene::Extract(EntityRegistry& registry, Frame& frame, const Camera& camera, RenderMesh* mesh)
    {
        float aspectRatio = m_renderService->GetFrameContext()->GetAspectRatio();

        Matrix4f proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
        RenderView view = RenderView::CreateFromCamera(camera, proj);

        m_vertexBuffers = mesh->GetVertexBuffers();
        m_indexBuffer = mesh->GetIndexBuffer();
        m_indirectBufferCount = 0;
        /*
        packet.ViewAllocation = frame.UniformAllocator.Allocate<RenderView>();
        frame.UniformAllocator.UpdateData(packet.ViewAllocation, view);

        packet.MeshAllocation = frame.StorageAllocator.AllocateArray<RenderInstance>(m_sponzaMesh->GetSubMeshCount());
        RenderInstanceLayout layout = RenderInstance::CreateLayout(frame.StorageAllocator.GetMinAligment());
        for (size_t i = 0; i < m_sponzaMesh->GetSubMeshCount(); i++)
        {
            const RenderSubMesh& renderSubMesh = m_sponzaMesh->GetSubMeshes()[i];
            layout.UpdateData(packet.MeshAllocation.Get<uint8_t>() + i * layout.Stride, renderSubMesh);
        }

        // Light allocation
        auto lightView = m_entityRegistry.View<TransformComponent, LightComponent>();
        packet.PointLightsAllocation = frame.UniformAllocator.AllocateArray<PointLight>(lightView.GetSize());

        size_t i = 0;
        for (const auto [entity, transform, light]: lightView)
        {
            packet.PointLightsAllocation.Get<PointLight>()[i++] = PointLight(transform.Position, light.Color);
        }

        auto directionalLightEntityView = m_entityRegistry.View<DirectionalLight>();
        Entity directionalLightEntity = directionalLightEntityView.First();
        auto [_, directionalLightComponent] = directionalLightEntityView.GetComponents(directionalLightEntity);

        packet.DirectionalLightAllocation = frame.UniformAllocator.Allocate<DirectionalLight>();
        frame.UniformAllocator.UpdateData(packet.DirectionalLightAllocation, directionalLightComponent);

        packet.CountersAllocation = frame.UniformAllocator.Allocate<uint32_t>();
        packet.CountersAllocation.Update<uint32_t>(lightView.GetSize());
        */
    }

}