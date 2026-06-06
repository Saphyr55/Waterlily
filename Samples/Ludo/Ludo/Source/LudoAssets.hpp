#pragma once

#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    // Asset Registry
    inline const StringID LudoAssetRegistry = WL_SID("Assets/Registry.lar");

    // Sponza
    inline const StringID LudoAssetModelSponza = WL_SID("Assets/Models/Sponza.lca");

    // Source Shader filepath
    // TODO: Remove HLSL path, we shouln't compile this in the game.
    inline const StringID LudoAssetGLSLVertexShaderGBuffer = WL_SID("../../../Assets/Shaders/HLSL/GBuffer.vert.hlsl");
    inline const StringID LudoAssetGLSLFragmentShaderGBuffer = WL_SID("../../../Assets/Shaders/HLSL/GBuffer.frag.hlsl");
    inline const StringID LudoAssetGLSLVertexShaderLighting = WL_SID("../../../Assets/Shaders/HLSL/Lighting.vert.hlsl");
    inline const StringID LudoAssetGLSLFragmentShaderLighting = WL_SID("../../../Assets/Shaders/HLSL/Lighting.frag.hlsl");

    // Compiled SPIR-V shader filepath
    inline const StringID AssetSPVVertexShaderGBuffer = WL_SID("Assets/Shaders/SPV/GBuffer.vert.spv");
    inline const StringID AssetSPVFragmentShaderGBuffer = WL_SID("Assets/Shaders/SPV/GBuffer.frag.spv");
    inline const StringID AssetSPVVertexShaderLighting = WL_SID("Assets/Shaders/SPV/Lighting.vert.spv");
    inline const StringID AssetSPVFragmentShaderLigthing = WL_SID("Assets/Shaders/SPV/Lighting.frag.spv");

}// namespace Wl
