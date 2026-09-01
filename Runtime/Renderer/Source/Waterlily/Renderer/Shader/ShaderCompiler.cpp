#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <filesystem>

namespace Wl
{

    bool ExecuteCommandCompilingShader(StringRef inputFilepath,
                                       StringRef outputFilepath,
                                       StringRef spvFilePath,
                                       StringRef entryPoint,
                                       Shader::Stage stage,
                                       StringRef command)
    {
        WL_LOG_INFO("SPIRVShaderCompiler", "%s", command.GetData());

        int32_t result = std::system(command.GetData());
        if (result != 0)
        {
            WL_LOG_ERROR("SPIRVShaderCompiler", "Failed to compile shader file: %s", inputFilepath.data());
            return false;
        }

        WL_LOG_INFO("SPIRVShaderCompiler", "Successfully compiled shader file: %s", inputFilepath.data());
        WL_LOG_INFO("SPIRVShaderCompiler", "Creating Shader Asset File...");

        FileSystem& fileSystem = FileSystem::GetPlatform();

        FileResult spvFileResult = fileSystem.OpenWrite(spvFilePath.data());
        WL_RETURN_OBJECT_WHEN(!spvFileResult.HasValue(), false);

        SharedPtr<File> spvFileHandle = spvFileResult.GetValue();
        Shader shader(SPIRVShader(ShaderStageToRHI(stage), spvFileHandle->ReadAllBytes()), entryPoint, stage);
        spvFileHandle->Close();

        FileResult shaderFileResult = fileSystem.OpenWrite(outputFilepath.data());
        WL_RETURN_OBJECT_WHEN(!shaderFileResult.HasValue(), false);
        SharedPtr<File> shaderFileHandle = shaderFileResult.GetValue();

        WLCA::SerializeAsset(shaderFileHandle, &shader);

        shaderFileHandle->Close();

        WL_LOG_INFO("SPIRVShaderCompiler", "Output Shader Asset File: %s", outputFilepath.data());

        return true;
    }

    bool SPIRVShaderCompiler::CompileSlang(StringRef inputFilepath,
                                           StringRef outputFilepath,
                                           StringRef entryPoint,
                                           Shader::Stage stage,
                                           bool keepSpvFile)
    {
        std::filesystem::path outPath(outputFilepath.GetData());
        std::filesystem::path outDir = outPath.parent_path();
        if (!outDir.empty() && !std::filesystem::exists(outDir))
        {
            std::filesystem::create_directories(outDir);
        }

        String spvFilePath = outputFilepath.data();
        spvFilePath.Append(".spv");

        String command = "slangc";
        command.Append(" \"");
        command.Append(inputFilepath);
        command.Append("\" -target spirv");
        // command.Append(" -profile spirv_1_4");
        command.Append(" -preserve-params");
        command.Append(" -emit-spirv-directly");
        command.Append(" -fvk-use-entrypoint-name");
        command.Append(" -o \"");
        command.Append(spvFilePath);
        command.Append("\" -entry ");
        command.Append(entryPoint);
        command.Append(" -DWLSL_SPIRV");
        
        return ExecuteCommandCompilingShader(inputFilepath, outputFilepath, spvFilePath, entryPoint, stage, command);
    }

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

        String spvFilePath = outputFilepath.data();
        spvFilePath.Append(".spv");

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
            case Shader::Stage::None:
                break;
        }

        String command = "dxc";
        command.Append(" -spirv -T ");
        command.Append(stageFlag);
        command.Append(" -E ");
        command.Append(entryPoint);
        command.Append(" ");
        command.Append(inputFilepath);
        command.Append(" -Fo ");
        command.Append(spvFilePath);
        command.Append(" -fspv-preserve-bindings");
        command.Append(" -fspv-preserve-interface");
        command.Append(" -fspv-target-env=vulkan1.3");
        command.Append(" -fspv-extension=SPV_EXT_descriptor_indexing");

        return ExecuteCommandCompilingShader(inputFilepath, outputFilepath, spvFilePath, entryPoint, stage, command);
    }

}// namespace Wl