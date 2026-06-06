#pragma once

#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"

namespace Wl
{
    class FrameGraphPass;

    inline const StringID GBufferPassName = WL_SID("GBuffer");

    struct GBufferPassParameters
    {
        FrameGraphTextureHandle Position;
        FrameGraphTextureHandle Normal;
        FrameGraphTextureHandle Albedo;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;

        size_t DrawCount = 0;
        RenderAllocation* RenderViewAllocation = nullptr;
        RenderAllocation* MeshAllocation = nullptr;

        RenderMesh* Mesh = nullptr;
    };

    FrameGraphPass& GBufferPassCreate(PassContext& passContext,
                                      GraphicsPipelineProperties& pipeline,
                                      GBufferPassParameters& data);

}// namespace Wl
