#pragma once

#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    // Asset Registry
    inline const StringID LudoAssetRegistry = WL_SID("Assets/Registry.lar");

    // Sponza
    inline const StringID LudoAssetModelSponza = WL_SID("Assets/Models/Sponza.lca");

    // Source Shader filepath
    // TODO: Remove GLSL path, we shouln't compile this in the game.
    inline const StringID LudoAssetGLSLVertexShaderForward = WL_SID("../../../Assets/Shaders/GLSL/forward.vert");
    inline const StringID LudoAssetGLSLFragmentShaderForward = WL_SID("../../../Assets/Shaders/GLSL/forward.frag");
    inline const StringID LudoAssetGLSLVertexShaderUI = WL_SID("../../../Assets/Shaders/GLSL/ui.vert");
    inline const StringID LudoAssetGLSLFragmentShaderUI = WL_SID("../../../Assets/Shaders/GLSL/ui.frag");

    // Compiled SPIR-V shader filepath
    inline const StringID LudoAssetSPVVertexShaderForward = WL_SID("Assets/Shaders/SPV/forward.vert.spv");
    inline const StringID LudoAssetSPVFragmentShaderForward = WL_SID("Assets/Shaders/SPV/forward.frag.spv");
    inline const StringID LudoAssetSPVVertexShaderUI = WL_SID("Assets/Shaders/SPV/ui.vert.spv");
    inline const StringID LudoAssetSPVFragmentShaderUI = WL_SID("Assets/Shaders/SPV/ui.frag.spv");

}// namespace Wl
