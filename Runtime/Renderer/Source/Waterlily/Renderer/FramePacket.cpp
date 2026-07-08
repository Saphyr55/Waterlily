#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Proxy/RenderProxy.hpp"

namespace Wl
{

    FramePacket FramePacketManager::ExtractPacket(const ViewData& view, const SharedPtr<RenderMesh>& mesh, uint32_t drawCount)
    {
        FramePacket packet;

        packet.View = view;
        packet.Mesh = mesh;
        packet.DrawCount = drawCount;

        return packet;
    }

    void FramePacketManager::PrepareFrame(FramePacket& packet, Frame& frame)
    {
        packet.ViewAllocation = frame.UniformAllocator.Allocate<ViewData>();
        frame.UniformAllocator.UpdateData(packet.ViewAllocation, packet.View);

        packet.MeshAllocation = frame.StorageAllocator.AllocateArray<RenderInstance>(packet.Mesh->GetSubMeshCount());
        RenderInstanceLayout layout = RenderInstance::CreateLayout(frame.StorageAllocator.GetMinAligment());
        for (size_t i = 0; i < packet.Mesh->GetSubMeshCount(); i++)
        {
            const RenderSubMesh& renderSubMesh = packet.Mesh->GetSubMeshes()[i];
            layout.UpdateData(packet.MeshAllocation.Get<uint8_t>() + i * layout.Stride, renderSubMesh);
        }
    }

}// namespace Wl
