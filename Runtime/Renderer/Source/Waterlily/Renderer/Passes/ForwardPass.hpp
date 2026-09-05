#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Passes/PassContext.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"


namespace Wl
{

    inline const StringID LudoForwardPassName = WL_SID("Forward");

    struct ForwardPassParameters
    {
        FrameGraphTextureHandle Color;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;

        RenderAllocation* LightAllocation;
    };

    WL_RENDERER_API FrameGraphPass& ForwardPassCreate(PassContext& passContext,
                                                      FramePacket& packet,
                                                      GraphicsPipelineState& pipeline,
                                                      ForwardPassParameters& params);

}// namespace Wl
