#include "Shader.hpp"

namespace Wl
{

    RHIShaderStage ShaderStageToRHI(Shader::Stage stage)
    {
        switch (stage)
        {
            case Shader::Stage::Vertex:
                return RHIShaderStage::Vertex;
            case Shader::Stage::Fragment:
                return RHIShaderStage::Fragment;
            case Shader::Stage::Compute:
                return RHIShaderStage::Compute;
            case Shader::Stage::None:
            default:
                return RHIShaderStage::None;
        }
        WL_CRASH("Missing Shader::Stage implementation.");
    }
    
    void operator<<(OutputStream& stream, const Shader& asset)
    {
        const Array<uint8_t>& bytecode = asset.GetSPIRVShader().GetByteCode();
        stream << static_cast<uint32_t>(bytecode.GetSizeInBytes());
        stream.Write(bytecode.GetData(), bytecode.GetSizeInBytes());
        stream << asset.GetEntryPoint();
        stream << static_cast<uint8_t>(asset.GetStage());
    }

    void operator>>(InputStream& stream, Shader& asset)
    {
        uint32_t bytecodeSize = 0;
        Array<uint8_t> bytecode;
        String entryPoint;
        uint8_t stageByte = 0;
        
        stream >> bytecodeSize;

        bytecode.Resize(bytecodeSize);
        stream.Read(bytecode.GetData(), bytecodeSize);
        stream >> entryPoint;
        stream >> stageByte;

        Shader::Stage stage = static_cast<Shader::Stage>(stageByte);

        asset = Shader(SPIRVShader(ShaderStageToRHI(stage), bytecode), entryPoint, stage);
    }

}