#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <filesystem>

namespace Wl
{

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
            case Stage::Geometry:
                stageFlag = "-V -S geom";
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