#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"

#include <filesystem>

namespace Wl
{

    bool SPIRVShaderCompiler::CompileHLSL(StringRef inputFilepath, 
                                          StringRef outputFilepath, 
                                          StringRef entryPoint, 
                                          Shader::Stage stage)
    {
        std::filesystem::path outPath(outputFilepath.GetData());
        std::filesystem::path outDir = outPath.parent_path();
        if (!outDir.empty() && !std::filesystem::exists(outDir))
        {
            std::filesystem::create_directories(outDir);
        }

        String stageFlag;
        switch (stage)
        {
            case Shader::Stage::Vertex:
                stageFlag = "vs_6_5";
                break;
            case Shader::Stage::Fragment:
                stageFlag = "ps_6_5";
                break;
            case Shader::Stage::Compute:
                stageFlag = "cs_6_5";
                break;
        }
        
        String cmd = "dxc";
        cmd.Append(" -spirv -T ");
        cmd.Append(stageFlag);
        cmd.Append(" -E ");
        cmd.Append(entryPoint);
        cmd.Append(" ");
        cmd.Append(inputFilepath);
        cmd.Append(" -Fo ");
        cmd.Append(outputFilepath);
        cmd.Append(" -fspv-preserve-bindings");
        cmd.Append(" -fspv-preserve-interface");
        cmd.Append(" -fspv-target-env=vulkan1.3");
        cmd.Append(" -fspv-extension=SPV_EXT_descriptor_indexing");

        WL_LOG_INFO("SPIRVShaderCompiler", "%s", cmd.GetData());

        int32_t result = std::system(cmd.GetData());
        if (result != 0)
        {
            WL_LOG_ERROR("SPIRVShaderCompiler", "Failed to compile shader file: %s", inputFilepath.data());
            return false;
        }

        WL_LOG_INFO("SPIRVShaderCompiler", "Successfully compiled shader file: %s", inputFilepath.data());
        
        WL_LOG_INFO("SPIRVShaderCompiler", "Creating Shader Asset File...");

        FileSystem& fileSystem = FileSystem::GetPlatform();

        FileResult vertexFileOpenError = fileSystem.OpenWrite(outputFilepath.data());
        WL_RETURN_OBJECT_WHEN(!vertexFileOpenError.HasValue(), false);

        SharedPtr<File> vertexFileHandle = vertexFileOpenError.GetValue();
        
        Shader shader(SPIRVShader(ShaderStageToRHI(stage), vertexFileHandle->ReadAllBytes()), entryPoint, stage);

        vertexFileHandle->Seek(0);
        WLCA::SerializeAsset(vertexFileHandle, &shader);
        vertexFileHandle->Close();

        WL_LOG_INFO("SPIRVShaderCompiler", "Output Shader Asset File: %s", outputFilepath.data());

        return true;
    }

}// namespace Wl