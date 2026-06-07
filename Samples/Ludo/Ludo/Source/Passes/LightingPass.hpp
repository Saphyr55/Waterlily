#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Passes/PassContext.hpp"

namespace Wl
{
    inline const StringID LightingPassName = WL_SID("Lighting");

    struct FrameGraphPass;

    struct LightingPassParameters
    {
        FrameGraphTextureHandle Color;
        FrameGraphTextureHandle Position;
        FrameGraphTextureHandle Normal;
        FrameGraphTextureHandle Albedo;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;

        RenderAllocation* MeshAllocation;
        RenderMesh* Mesh;

        RenderAllocation* LightAllocation;
        RenderAllocation* RenderViewAllocation;
    };

    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineProperties& pipeline, LightingPassParameters& params);

}
