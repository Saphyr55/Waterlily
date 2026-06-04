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

        bool success = SPIRVShaderCompiler::CompileGLSL(LudoAssetGLSLVertexShaderForward.GetText(),
                                                        LudoAssetSPVVertexShaderForward.GetText(),
                                                        SPIRVShaderCompiler::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileGLSL(LudoAssetGLSLFragmentShaderForward.GetText(),
                                                              LudoAssetSPVFragmentShaderForward.GetText(),
                                                              SPIRVShaderCompiler::Stage::Fragment);

        success = success && SPIRVShaderCompiler::CompileGLSL(LudoAssetGLSLVertexShaderUI.GetText(),
                                                              LudoAssetSPVVertexShaderUI.GetText(),
                                                              SPIRVShaderCompiler::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileGLSL(LudoAssetGLSLFragmentShaderUI.GetText(),
                                                              LudoAssetSPVFragmentShaderUI.GetText(),
                                                              SPIRVShaderCompiler::Stage::Fragment);

        return success;
    }

    void ReloadShaders(SharedPtr<RHIDevice> device,
                       SharedPtr<PipelineManager> pipelineManager,
                       HashMap<StringID, GraphicsPipelineProperties*>& propsMap)
    {
        device->WaitIdle();

        CompileShaders();
        for (const auto& [name, props]: propsMap)
        {
            pipelineManager->Recreate(name, *props);
        }
    }

}// namespace Wl
