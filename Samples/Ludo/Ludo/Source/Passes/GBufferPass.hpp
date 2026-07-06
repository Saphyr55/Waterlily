#pragma once

#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"

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
    };

    FrameGraphPass& GBufferPassCreate(PassContext& passContext,
                                      FramePacket& packet,
                                      GraphicsPipelineState& pipeline,
                                      GBufferPassParameters& data);

}// namespace Wl
