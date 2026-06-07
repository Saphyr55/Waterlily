#include "LudoShaders.hpp"

#include "LudoAssets.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"

namespace Wl
{

    bool CompileShaders()
    {
        FileSystem& fileSystem = FileSystem::GetPlatform();

        bool success = SPIRVShaderCompiler::CompileHLSL(LudoAssetShaderGBuffer.GetText(),
                                                        AssetSPVVertexShaderGBuffer.GetText(),
                                                        "VSMain",
                                                        SPIRVShaderCompiler::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(LudoAssetShaderGBuffer.GetText(),
                                                              AssetSPVFragmentShaderGBuffer.GetText(),
                                                              "FSMain",
                                                              SPIRVShaderCompiler::Stage::Fragment);

        success = success && SPIRVShaderCompiler::CompileHLSL(LudoAssetShaderLighting.GetText(),
                                                              AssetSPVVertexShaderLighting.GetText(),
                                                              "VSMain",
                                                               SPIRVShaderCompiler::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(LudoAssetShaderLighting.GetText(),
                                                              AssetSPVFragmentShaderLigthing.GetText(),
                                                              "FSMain",
                                                              SPIRVShaderCompiler::Stage::Fragment);
        return success;
    }

    void ReloadShaders(SharedPtr<RHIDevice> device,
                       SharedPtr<PipelineManager> pipelineManager,
                       HashMap<StringID, GraphicsPipelineProperties*>& propsMap)
    {
        device->WaitIdle();

        CompileShaders();
        for (auto [name, props]: propsMap)
        {
            pipelineManager->Recreate(name, *props);
        }
    }

}// namespace Wl
