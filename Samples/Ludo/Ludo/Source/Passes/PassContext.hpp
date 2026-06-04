#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"

namespace Wl
{

    struct PassContext
    {
        SharedPtr<FrameGraph> FrameGraph;
        SharedPtr<PipelineManager> PipelineManager;
        SharedPtr<TextureRegistry> TextureRegistry;
        SharedPtr<MaterialRegistry> MaterialRegistry;
    };

}// namespace Wl
