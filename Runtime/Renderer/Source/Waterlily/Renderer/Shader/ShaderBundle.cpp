#include "ShaderBundle.hpp"

namespace Wl
{

    ShaderBundle::ShaderBundle(const SharedPtr<AssetRegistry>& assetRegistry,
                               const SharedPtr<AssetManager>& assetManager,
                               const SharedPtr<PipelineManager>& pipelineManager)
        : m_assetRegistry(assetRegistry)
        , m_assetManager(assetManager)
        , m_pipelineManager(pipelineManager)
    {
    }
    
    ShaderGraphicsPass& ShaderBundle::GetShaderGraphicsPass(StringID passName)
    {
        return m_graphicsShaders[passName];
    }

    void ShaderBundle::RegisterGraphicsPass(StringID passName, StringID vertexName, StringID fragmentName)
    {
        ShaderGraphicsPass shader = {};
        shader.VertexAssetHandle = m_assetRegistry->CreateAsset(AssetType_Shader, vertexName);
        shader.FragmentAssetHandle = m_assetRegistry->CreateAsset(AssetType_Shader, fragmentName);
        m_graphicsShaders[passName] = shader;
    }

    void ShaderBundle::LoadInternal(bool reload)
    {
        for (auto [name, shader]: m_graphicsShaders)
        {
            shader.PipelineState.VertexShader = m_assetManager->GetAsset<Shader>(shader.VertexAssetHandle, reload);
            shader.PipelineState.FragmentShader = m_assetManager->GetAsset<Shader>(shader.FragmentAssetHandle, reload);
        }
    }

}// namespace Wl
