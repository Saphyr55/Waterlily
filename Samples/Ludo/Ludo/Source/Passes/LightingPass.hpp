#pragma once

#include "Passes/PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{
    inline const StringID LightingPassName = WL_SID("Lighting");

    inline const StringID LightingShaderAssetPath = WL_SID("../../../Assets/Shaders/Lighting.hlsl");

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
        RenderMesh* Mesh;

        RenderAllocation* LightAllocation;
        RenderAllocation* CountersAllocation;
        RenderAllocation* RenderViewAllocation;
    };

    FrameGraphPass& LightingPassCreate(PassContext& passContext, GraphicsPipelineState& pipeline, LightingPassParameters& params);

}// namespace Wl
