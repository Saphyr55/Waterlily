#pragma once

#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/IO/PlatformFileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/EngineService.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/DeviceFactory.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Shader/ShaderBundle.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"


namespace Wl
{
    inline StringID RenderServiceName = WL_SID("RenderService");

    constexpr RHIGraphicsAPI DefaultGraphicsAPI = RHIGraphicsAPI::Vulkan;

    constexpr SRGIndex SRGIndexDefaultMaterials = 3;
    constexpr SRGBinding SRGBindingDefaultMaterials = 0;

    constexpr SRGIndex SRGIndexDefaultTextures = 2;
    constexpr SRGBinding SRGBindingDefaultTextures = 0;

    struct RenderServiceConfig
    {
        SharedPtr<Window> RenderWindow;

        SharedPtr<AssetManager> Assets;
        FileSystem& AssetFileSystem;

        RHIGraphicsAPI GraphicsAPI = DefaultGraphicsAPI;

        SRGIndex SRGIndexMaterials = SRGIndexDefaultMaterials;
        SRGBinding SRGBindingTextures = SRGBindingDefaultTextures;

        SRGIndex SRGIndexTextures = SRGIndexDefaultTextures;
        SRGBinding SRGBindingMaterials = SRGBindingDefaultMaterials;

        size_t FrameAllocationSize = 16 * WL_MB;
        size_t StagingBufferSize = 16 * WL_MB;
        size_t StorageBufferSize = 16 * WL_MB;
        size_t UniformBufferSize = 16 * WL_MB;

        RenderServiceConfig(const SharedPtr<Window>& window, const SharedPtr<AssetManager>& assetManager, FileSystem& assetFileSystem)
            : RenderWindow(window)
            , Assets(assetManager)
            , AssetFileSystem(assetFileSystem)
        {
        }
    };

    class WL_RENDERER_API RenderService : public EngineService
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        void Resize(float width, float height);

        RHIPipeline* GetOrCreatePipeline(FrameGraphPass& pass, GraphicsPipelineState& state);

    public:
        inline SharedPtr<RHIDevice> GetDevice()
        {
            return m_device;
        }

        inline SharedPtr<FrameContext> GetFrameContext()
        {
            return m_frameContext;
        }

        inline SharedPtr<FrameGraph> GetFrameGraph()
        {
            return m_frameGraph;
        }

        inline SharedPtr<PipelineManager> GetPipelineManager()
        {
            return m_pipelineManager;
        }

        inline SharedPtr<ShaderBundle> GetShaderBundle()
        {
            return m_shaderBundle;
        }

        inline SharedPtr<TextureRegistry> GetTextureRegistry()
        {
            return m_textureRegistry;
        }

        inline SharedPtr<MaterialRegistry> GetMaterialRegistry()
        {
            return m_materialRegistry;
        }

        inline SharedPtr<Window> GetWindow()
        {
            return m_config.RenderWindow;
        }

    public:
        RenderService(const RenderServiceConfig& config);
        virtual ~RenderService() = default;

    private:
        RenderServiceConfig m_config;
        SharedPtr<RHIDevice> m_device;
        SharedPtr<FrameContext> m_frameContext;
        SharedPtr<FrameGraph> m_frameGraph;
        SharedPtr<PipelineManager> m_pipelineManager;
        SharedPtr<ShaderBundle> m_shaderBundle;
        SharedPtr<TextureRegistry> m_textureRegistry;
        SharedPtr<MaterialRegistry> m_materialRegistry;
    };
}// namespace Wl