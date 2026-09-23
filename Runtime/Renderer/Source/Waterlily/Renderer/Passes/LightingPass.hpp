#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Passes/PassContext.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{
    inline const StringID LightingPassName = StringID("Lighting");

    struct FrameGraphPass;

    struct LightingPassParameters
    {
        FrameGraphTextureHandle Color;
        FrameGraphTextureHandle Position;
        FrameGraphTextureHandle Normal;
        FrameGraphTextureHandle Albedo;
        FrameGraphTextureHandle MetallicRoughness;
        FrameGraphTextureHandle ShadowMap;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;

        RenderAllocation* DirectionalLightSpaceAlloc; 
    };

    WL_RENDERER_API FrameGraphPass& LightingPassCreate(PassContext& passContext,
                                                       FramePacket& packet,
                                                       ComputePipelineState& pipeline,
                                                       LightingPassParameters& params);

}// namespace Wl
