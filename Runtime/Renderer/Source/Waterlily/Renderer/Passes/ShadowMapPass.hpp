#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Passes/PassContext.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"


namespace Wl
{

    inline const StringID ShadowMapPassName = WL_SID("ShadowMap");

    struct ShadowMapPassParameters
    {
        FrameGraphTextureHandle ShadowMap;
        FrameGraphBufferHandle Indirect;
        RenderAllocation* DirectionalLightViewAllocation;
    };

    WL_RENDERER_API FrameGraphPass& ShadowMapPassCreate(PassContext& passContext,
                                                        FramePacket& packet,
                                                        GraphicsPipelineState& pipeline,
                                                        ShadowMapPassParameters& params);

}// namespace Wl
