#pragma once

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
        FramePacket ExtractPacket(const ViewData& view, const SharedPtr<RenderMesh>& mesh, uint32_t drawCount);

        void PrepareFrame(FramePacket& packet, Frame& frame);
    };

}// namespace Wl