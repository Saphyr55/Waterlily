#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"

namespace Wl
{
    inline constexpr SRGIndex SRGIndexGlobal = 0;
    inline constexpr SRGBinding SRGBindingGlobalView = 0;
    inline constexpr SRGBinding SRGBindingGlobalPointLights = 1;
    inline constexpr SRGBinding SRGBindingGlobalDirectionalLight = 2;
    inline constexpr SRGBinding SRGBindingGlobalCounters = 3;

    inline constexpr SRGIndex SRGIndexRenderInstance = 1;
    inline constexpr SRGBinding SRGBindingRenderInstance = 0;

    inline constexpr SRGIndex SRGIndexTextures = 2;
    inline constexpr SRGBinding SRGBindingTextures = 0;

    inline constexpr SRGIndex SRGIndexMaterials = 3;
    inline constexpr SRGBinding SRGBindingMaterials = 0;

    inline constexpr SRGIndex SRGIndexGBuffer = 4;
    inline constexpr SRGBinding SRGBindingGBufferPosition = 0;
    inline constexpr SRGBinding SRGBindingGBufferNormal = 1;
    inline constexpr SRGBinding SRGBindingGBufferAlbedo = 2;

    struct PassContext
    {
        SharedPtr<FrameGraph> FrameGraph;
        SharedPtr<PipelineManager> PipelineManager;
        SharedPtr<TextureRegistry> TextureRegistry;
        SharedPtr<MaterialRegistry> MaterialRegistry;
    };

}// namespace Wl
