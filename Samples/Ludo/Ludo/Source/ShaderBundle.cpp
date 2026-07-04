#include "ShaderBundle.hpp"
#include "Passes/GBufferPass.hpp"
#include "Passes/LightingPass.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"

namespace Wl
{

    void ShaderBundle::CreateAssets(SharedPtr<AssetRegistry> assetRegistry)
    {
        GBufferVertexShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, GBufferVertexShaderAssetURI);
        GBufferFragmentShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, GBufferFragmentShaderAssetURI);
        LightingVertexShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, LightingVertexShaderAssetURI);
        LightingFragmentShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, LightingFragmentShaderAssetURI);
    }

    bool ShaderBundle::LoadInternal(SharedPtr<AssetManager> assetManager, bool reload)
    {
        FileSystem& fileSystem = FileSystem::GetPlatform();

        bool success = SPIRVShaderCompiler::CompileHLSL(GBufferShaderAssetURI.GetText(),
                                                        GBufferVertexShaderAssetURI.GetText(),
                                                        "VSMain",
                                                        Shader::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(GBufferShaderAssetURI.GetText(),
                                                              GBufferFragmentShaderAssetURI.GetText(),
                                                              "FSMain",
                                                              Shader::Stage::Fragment);

        success = success && SPIRVShaderCompiler::CompileHLSL(LightingShaderAssetPath.GetText(),
                                                              LightingVertexShaderAssetURI.GetText(),
                                                              "VSMain",
                                                              Shader::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(LightingShaderAssetPath.GetText(),
                                                              LightingFragmentShaderAssetURI.GetText(),
                                                              "FSMain",
                                                              Shader::Stage::Fragment);

        GBufferPipelineProperties.VertexShader = assetManager->GetAsset<Shader>(GBufferVertexShaderAssetHandle, reload);
        GBufferPipelineProperties.FragmentShader = assetManager->GetAsset<Shader>(GBufferFragmentShaderAssetHandle, reload);
        LightingPipelineProperties.VertexShader = assetManager->GetAsset<Shader>(LightingVertexShaderAssetHandle, reload);
        LightingPipelineProperties.FragmentShader = assetManager->GetAsset<Shader>(LightingFragmentShaderAssetHandle, reload);

        return success &&
               GBufferPipelineProperties.VertexShader &&
               GBufferPipelineProperties.FragmentShader &&
               LightingPipelineProperties.VertexShader &&
               LightingPipelineProperties.FragmentShader;
    }

}// namespace Wl
