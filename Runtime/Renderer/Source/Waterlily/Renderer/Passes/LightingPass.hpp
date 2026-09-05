#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Passes/PassContext.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

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
    };

    WL_RENDERER_API FrameGraphPass& LightingPassCreate(PassContext& passContext,
                                                       FramePacket& packet,
                                                       GraphicsPipelineState& pipeline,
                                                       LightingPassParameters& params);

}// namespace Wl
