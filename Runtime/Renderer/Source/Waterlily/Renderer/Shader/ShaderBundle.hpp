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

        void LoadAssets()
        {
            LoadInternal(false);
        }

        void ReloadAssets()
        {
            LoadInternal(true);
            for (auto [name, shader]: m_graphicsShaders)
            {
                m_pipelineManager->Recreate(name, shader.PipelineState);
            }
        }

    public:
        ShaderBundle(const SharedPtr<AssetRegistry>& assetRegistry,
                     const SharedPtr<AssetManager>& assetManager,
                     const SharedPtr<PipelineManager>& pipelineManager);

    private:
        void LoadInternal(bool reload);

    private:
        SharedPtr<AssetRegistry> m_assetRegistry;
        SharedPtr<AssetManager> m_assetManager;
        SharedPtr<PipelineManager> m_pipelineManager;

        HashMap<StringID, ShaderGraphicsPass> m_graphicsShaders;
    };

}// namespace Wl