#include "Waterlily/Renderer/Proxy/MeshRenderProxy.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"

namespace Wl
{

    void MeshRenderProxy::PrepareFrame(FramePacket& packet, Frame& frame)
    {
        m_meshAllocation = frame.StorageAllocator.AllocateArray<RenderInstance>(m_mesh->GetSubMeshCount());
        RenderInstanceLayout layout = RenderInstance::CreateLayout(frame.StorageAllocator.GetMinAligment());
        for (size_t i = 0; i < m_mesh->GetSubMeshCount(); i++)
        {
            const RenderSubMesh& renderSubMesh = m_mesh->GetSubMeshes()[i];
            layout.UpdateData(m_meshAllocation.Get<uint8_t>() + i * layout.Stride, renderSubMesh);
        }
    }

}// namespace Wl
