#pragma once

#include "Waterlily/Core/String/StringID.hpp"

using namespace Wl;

namespace Ludo
{

    inline const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

    inline const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    inline const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");
    inline const StringID ShadowMapVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/ShadowMap.vert.wlca");
    inline const StringID ShadowMapFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/ShadowMap.frag.wlca");
    inline const StringID LightingComputeShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.comp.wlca");

}// namespace Ludo
