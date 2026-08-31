#include "RenderService.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/DeviceFactory.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"

namespace Wl
{

    void RenderService::OnStartup()
    {
        m_device = RHIDeviceFactory::Create(m_config.GraphicsAPI);
        m_device->Init(m_config.RenderWindow->GetNativeWindow());

        FrameContextInitInfo frameContextInitInfo = {};
        frameContextInitInfo.FrameAllocationSize = m_config.FrameAllocationSize;
        frameContextInitInfo.StagingBufferSize = m_config.StagingBufferSize;
        frameContextInitInfo.StorageBufferSize = m_config.StorageBufferSize;
        frameContextInitInfo.UniformBufferSize = m_config.UniformBufferSize;
        frameContextInitInfo.FrameWidth = m_config.RenderWindow->GetProperties().Width;
        frameContextInitInfo.FrameHeight = m_config.RenderWindow->GetProperties().Height;

        m_frameContext = MakeShared<FrameContext>(m_device);
        m_frameContext->Init(frameContextInitInfo);

        m_textureRegistry = MakeShared<TextureRegistry>(m_device, *m_config.Assets, m_config.SRGBindingTextures);
        m_materialRegistry = MakeShared<MaterialRegistry>(m_device, m_config.SRGBindingMaterials);
        m_pipelineManager = MakeShared<PipelineManager>(m_device, m_frameContext->GetSRGLayoutCache(), m_config.AssetFileSystem);
        m_shaderBundle = MakeShared<ShaderBundle>(m_config.Assets->GetRegistry(), m_config.Assets, m_pipelineManager);
        m_frameGraph = MakeShared<FrameGraph>(m_frameContext);
    }

    void RenderService::Resize(float width, float height)
    {
        m_frameContext->Resize(width, height);
        m_frameGraph->Resize();
    }

    void RenderService::OnShutdown()
    {
        m_frameGraph->Destroy();

        m_pipelineManager->Destroy();

        m_materialRegistry->Destroy();
        m_textureRegistry->Destroy();

        m_frameContext->Destroy();
        m_device->Destroy();
    }

    RHIPipeline* RenderService::GetOrCreatePipeline(FrameGraphPass& pass, GraphicsPipelineState& state)
    {
        state.SRGLayouts[m_config.SRGIndexTextures] = m_textureRegistry->GetSRGLayout();
        state.SRGLayouts[m_config.SRGIndexMaterials] = m_materialRegistry->GetSRGLayout();
        state.CullMode = state.CullMode;
        state.Viewport = state.Viewport;
        state.Scissor = state.Scissor;
        state.VertexShader = state.VertexShader;
        state.FragmentShader = state.FragmentShader;
        state.RenderPass = m_frameGraph->GetRenderPass(pass.GetName());

        return m_pipelineManager->GetOrCreate(pass.GetName(), state);
    }

    RenderService::RenderService(const RenderServiceConfig& config)
        : m_config(config)
    {
    }

}// namespace Wl