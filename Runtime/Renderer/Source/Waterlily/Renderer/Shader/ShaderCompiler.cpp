#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <filesystem>

namespace Wl
{

    bool SPIRVShaderCompiler::CompileHLSL(StringRef inputFilepath, StringRef outputFilepath, Stage stage)
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
            case Stage::Vertex:
                stageFlag = "vs_6_5";
                break;
            case Stage::Fragment:
                stageFlag = "ps_6_5";
                break;
            case Stage::Compute:
                stageFlag = "cs_6_5";
                break;
        }
        
        String cmd = "dxc";
        cmd.Append(" -spirv -T ");
        cmd.Append(stageFlag);
        cmd.Append(" -E main ");
        cmd.Append(inputFilepath);
        cmd.Append(" -Fo ");
        cmd.Append(outputFilepath);
        cmd.Append(" -Od ");
        cmd.Append(" -fspv-target-env=vulkan1.3");
        cmd.Append(" -fspv-extension=SPV_EXT_descriptor_indexing");

        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("%s", cmd.GetData()));

        int32_t result = std::system(cmd.GetData());
        if (result != 0)
        {
            WL_LOG_ERROR("[SPIRVShaderCompiler]", Wl::Format("Failed to compile shader file: %s", inputFilepath.data()));
            return false;
        }

        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("Successfully compiled shader file: %s", inputFilepath.data()));
        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("Output spv file: %s", outputFilepath.data()));

        return true;
    }

    bool SPIRVShaderCompiler::CompileGLSL(StringRef inputFilepath, StringRef outputFilepath, Stage stage)
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
            case Stage::Vertex:
                stageFlag = "-V -S vert";
                break;
            case Stage::Fragment:
                stageFlag = "-V -S frag";
                break;
            case Stage::Compute:
                stageFlag = "-V -S comp";
                break;
        }

        String cmd = "glslangValidator ";
        cmd.Append(stageFlag);
        cmd.Append(" ");
        cmd.Append(inputFilepath);
        cmd.Append(" -o ");
        cmd.Append(outputFilepath);
        cmd.Append(" -Os");

        int32_t result = std::system(cmd.GetData());
        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("%s", cmd.GetData()));
        if (result != 0)
        {
            WL_LOG_ERROR("[SPIRVShaderCompiler]", Wl::Format("Failed to compile shader file: %s", inputFilepath.data()));
            return false;
        }

        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("Successfully compiled shader file: %s", inputFilepath.data()));
        WL_LOG_INFO("[SPIRVShaderCompiler]", Wl::Format("Output spv file: %s", outputFilepath.data()));

        return true;
    }

}// namespace Wl