#pragma once

#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{
    inline const StringID LightingPassName = WL_SID("Lighting");

    struct FrameGraphPass;

    class LightingShader
    {
    private:
        GraphicsPipelineState m_pipelineState;
    };

    struct LightingPassParameters
    {
        FrameGraphTextureHandle Color;
        FrameGraphTextureHandle Position;
        FrameGraphTextureHandle Normal;
        FrameGraphTextureHandle Albedo;
        FrameGraphTextureHandle DepthStencil;
        FrameGraphBufferHandle Indirect;

        RenderAllocation* MeshAllocation;

        RenderAllocation* PointLightsAllocation;
        RenderAllocation* DirectionalLightAllocation;
        RenderAllocation* CountersAllocation;
        RenderAllocation* ViewAllocation;
    };

    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineState& pipeline, LightingPassParameters& params);

}// namespace Wl
