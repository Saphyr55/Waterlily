#pragma once

#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"


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
        RenderAllocation* CountersAllocation;
        RenderAllocation* RenderViewAllocation;
    };

    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineProperties& pipeline, LightingPassParameters& params);

}// namespace Wl
