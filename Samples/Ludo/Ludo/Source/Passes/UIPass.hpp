#pragma once

#include "LudoExports.hpp"
#include "PassContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    struct UIPassParameters
    {
        FrameGraphTextureHandle Color;
        RenderMesh* Mesh;
    };

    LUDO_API FrameGraphPass& UIPassCreate(PassContext& pass_context,
                                          GraphicsPipelineProperties& pipeline,
                                          UIPassParameters& params);

}// namespace Wl
