#pragma once

#include "LudoExports.hpp"
#include "Passes/passContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"

namespace Wl
{

    struct ForwardPassParameters
    {
        FrameGraphTextureHandle Color;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;
        size_t DrawCount;

        RenderAllocation* MeshAllocation;
        RenderMesh* Mesh;

        RenderAllocation* LightAllocation;
        RenderAllocation* RenderViewAllocation;
    };

    LUDO_API FrameGraphPass& ForwardPassCreate(PassContext& context,
                                               GraphicsPipelineProperties& pipeline,
                                               ForwardPassParameters& params);

}// namespace Wl
