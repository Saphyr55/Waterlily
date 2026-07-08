#pragma once

#include "LudoExports.hpp"
#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"

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

    LUDO_API FrameGraphPass& ForwardPassCreate(PassContext& passContext,
                                               FramePacket& packet,
                                               GraphicsPipelineState& pipeline,
                                               ForwardPassParameters& params);

}// namespace Wl
