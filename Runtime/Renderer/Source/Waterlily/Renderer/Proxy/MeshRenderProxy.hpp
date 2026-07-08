#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Proxy/RenderProxy.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"


namespace Wl
{

    class MeshRenderProxy : public RenderProxy
    {
    public:
        virtual void PrepareFrame(FramePacket& packet, Frame& frame) override;

    public:
        MeshRenderProxy(const SharedPtr<RenderMesh>& mesh)
            : m_mesh(mesh)
        {
        }

    private:
        SharedPtr<RenderMesh> m_mesh;
        RenderAllocation m_meshAllocation;
    };

}// namespace Wl