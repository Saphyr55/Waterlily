#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{
    
    inline const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    inline const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");
    inline const StringID LightingVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.vert.wlca");
    inline const StringID LightingFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.frag.wlca");

    struct ShaderBundle
    {
    public:
        // GBuffer
        GraphicsPipelineState GBufferPipelineProperties = {};
        AssetHandle GBufferVertexShaderAssetHandle;
        AssetHandle GBufferFragmentShaderAssetHandle;

        // Lighting
        GraphicsPipelineState LightingPipelineProperties = {};
        AssetHandle LightingVertexShaderAssetHandle;
        AssetHandle LightingFragmentShaderAssetHandle;

    public:
        void CreateAssets(SharedPtr<AssetRegistry> assetRegistry);

        bool LoadAssets(const SharedPtr<AssetManager>& assetManager)
        {
            return LoadInternal(assetManager, false);
        }

        bool ReloadAssets(const SharedPtr<AssetManager>& assetManager)
        {
            return LoadInternal(assetManager, true);
        }

    private:
        bool LoadInternal(SharedPtr<AssetManager> assetManager, bool reload);
    };

}// namespace Wl