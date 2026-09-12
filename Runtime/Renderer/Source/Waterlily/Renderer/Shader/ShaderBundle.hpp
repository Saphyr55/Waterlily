#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    struct ShaderGraphicsPass
    {
        GraphicsPipelineState PipelineState;
        AssetHandle VertexAssetHandle;
        AssetHandle FragmentAssetHandle;
    };

    class WL_RENDERER_API ShaderBundle
    {
    public:
        void RegisterGraphicsPass(StringID passName, StringID vertexName, StringID fragmentName);

        ShaderGraphicsPass& GetShaderGraphicsPass(StringID passName);

        void LoadAssets();
        void ReloadAssets();

    public:
        ShaderBundle(const SharedPtr<RHIDevice>& device,
                     const SharedPtr<AssetRegistry>& assetRegistry,
                     const SharedPtr<AssetManager>& assetManager,
                     const SharedPtr<PipelineManager>& pipelineManager);

    private:
        void LoadInternal(bool reload);

    private:
        SharedPtr<RHIDevice> m_device;
        SharedPtr<AssetRegistry> m_assetRegistry;
        SharedPtr<AssetManager> m_assetManager;
        SharedPtr<PipelineManager> m_pipelineManager;

        HashMap<StringID, ShaderGraphicsPass> m_graphicsShaders;
    };

}// namespace Wl