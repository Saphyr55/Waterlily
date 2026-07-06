#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"

namespace Wl
{

    inline constexpr uint32_t SRGIndexGlobal = 0;
    inline constexpr uint32_t SRGBindingGlobalView = 0;
    inline constexpr uint32_t SRGBindingGlobalPointLights = 1;
    inline constexpr uint32_t SRGBindingGlobalDirectionalLight = 2;
    inline constexpr uint32_t SRGBindingGlobalCounters = 3;

    inline constexpr uint32_t SRGIndexRenderInstance = 1;
    inline constexpr uint32_t SRGBindingRenderInstance = 0;

    inline constexpr uint32_t SRGIndexTextures = 2;
    inline constexpr uint32_t SRGBindingTextures = 0;

    inline constexpr uint32_t SRGIndexMaterials = 3;
    inline constexpr uint32_t SRGBindingMaterials = 0;

    inline constexpr uint32_t SRGIndexGBuffer = 4;
    inline constexpr uint32_t SRGBindingGBufferPosition = 0;
    inline constexpr uint32_t SRGBindingGBufferNormal = 1;
    inline constexpr uint32_t SRGBindingGBufferAlbedo = 2;

    struct PassContext
    {
        SharedPtr<FrameGraph> FrameGraph;
        SharedPtr<PipelineManager> PipelineManager;
        SharedPtr<TextureRegistry> TextureRegistry;
        SharedPtr<MaterialRegistry> MaterialRegistry;
    };

}// namespace Wl
