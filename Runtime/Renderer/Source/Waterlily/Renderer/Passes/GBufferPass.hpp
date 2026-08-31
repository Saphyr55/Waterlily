#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Passes/PassContext.hpp"

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

    WL_RENDERER_API FrameGraphPass& GBufferPassCreate(PassContext& passContext,
                                                      FramePacket& packet,
                                                      GraphicsPipelineState& pipeline,
                                                      GBufferPassParameters& data);

}// namespace Wl
