#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/View.hpp"

namespace Wl
{

    class RenderProxy;
    class Frame;

    struct FramePacket
    {
        ViewData View;
        RenderAllocation ViewAllocation;

        SharedPtr<RenderMesh> Mesh;
        RenderAllocation MeshAllocation;

        uint32_t DrawCount = 0;
    };

    class WL_RENDERER_API FramePacketManager
    {
    public:
        void RegisterProxy(StringID name, SharedPtr<RenderProxy> proxy);
        void UnregisterProxy(StringID name);

        FramePacket ExtractPacket(const ViewData& view, const SharedPtr<RenderMesh>& mesh, uint32_t drawCount);
        
        void PrepareFrame(FramePacket& packet, Frame& frame);

    private:
        HashMap<StringID, SharedPtr<RenderProxy>> m_proxies;
    };

}// namespace Wl