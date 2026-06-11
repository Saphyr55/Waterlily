#pragma once

#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/RHI/CompiledShader.hpp"

namespace Wl
{

    inline const StringID AssetType_Shader = WL_SID("Shader");

	class WL_RENDERER_API Shader : public Asset
	{
    public:
        enum class Stage : uint8_t
        {
            None = 0,
            Vertex,
            Fragment,
            Compute,
        };

        inline const SPIRVShader& GetSPIRVShader() const
        {
            return m_spirv;
        }

        inline StringRef GetEntryPoint() const
        {
            return m_entryPoint;
        }

        inline Stage GetStage() const
        {
            return m_stage;
        }

        inline bool IsVertexShader() const
        {
            return m_stage == Stage::Vertex;
        }

        inline bool IsFragmentShader() const
        {
            return m_stage == Stage::Fragment;
        }

        inline bool IsComputeShader() const
        {
            return m_stage == Stage::Compute;
        }

    public:
        Shader()
            : Asset(AssetType_Shader)
            , m_stage(Stage::None)
        {
        }

        Shader(const SPIRVShader& spirv, StringRef entryPoint, Stage stage)
            : Asset(AssetType_Shader)
            , m_spirv(spirv)
            , m_entryPoint(entryPoint)
            , m_stage(stage)
        {
        }

        Shader(SPIRVShader&& spirv, StringRef entryPoint, Stage stage)
            : Asset(AssetType_Shader)
            , m_spirv(std::move(spirv))
            , m_entryPoint(entryPoint)
            , m_stage(stage)
        {
        }

    private : 
        SPIRVShader m_spirv;
        String m_entryPoint;
        Stage m_stage;
	};

    RHIShaderStage ShaderStageToRHI(Shader::Stage stage);

    WL_RENDERER_API void operator<<(OutputStream& stream, const Shader& asset);
    WL_RENDERER_API void operator>>(InputStream& stream, Shader& asset);

}