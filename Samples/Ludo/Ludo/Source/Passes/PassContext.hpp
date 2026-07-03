#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"

namespace Wl
{

    inline constexpr uint32_t GlobalSRGIndex = 0;
    inline constexpr uint32_t GlobalSRGRenderViewBinding = 0;
    inline constexpr uint32_t GlobalSRGLightBinding = 1;

    inline constexpr uint32_t RenderInstanceSRGIndex = 1;
    inline constexpr uint32_t RenderInstanceSRGBinding = 0;

    inline constexpr uint32_t LudoTextureGRGIndex = 2;
    inline constexpr uint32_t LudoTexturesSRGBinding = 0;

    inline constexpr uint32_t LudoMaterialsSRGIndex = 3;
    inline constexpr uint32_t LudoMaterialsSRGBinding = 0;

    struct PassContext
    {
        SharedPtr<FrameGraph> FrameGraph;
        SharedPtr<PipelineManager> PipelineManager;
        SharedPtr<TextureRegistry> TextureRegistry;
        SharedPtr<MaterialRegistry> MaterialRegistry;
    };

}// namespace Wl
