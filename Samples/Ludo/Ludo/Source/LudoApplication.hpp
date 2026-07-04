#pragma once

#include "LudoExports.hpp"
#include "ShaderBundle.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/Engine/ApplicationDelegate.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Pipeline.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Renderer/View.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PunctualLight.hpp"

namespace Wl
{

    class LUDO_API LudoApplicationDelegate : public ApplicationDelegate
    {
    public:
        virtual void OnStartup() override;

        virtual void OnRender() override;

        virtual void OnUpdate(double deltaTime) override;

        virtual void OnShutdown() override;

    public:
        LudoApplicationDelegate();
        ~LudoApplicationDelegate()
        {
            WL_LOG_DEBUG("Delegate", "Test descruction");
        }

    private:
        RHIPipeline* GetOrCreatePipeline(FrameGraphPass& pass,
                                         GraphicsPipelineState& state,
                                         RHICullModeFlags cullMode,
                                         const Viewport& viewport,
                                         const Rect2D& scissor);

    private:
        // Rendering.
        SharedPtr<Window> m_window;
        SharedPtr<RHIDevice> m_device;
        SharedPtr<FrameContext> m_frameContext;

        // Graphics.
        SharedPtr<TextureRegistry> m_textureRegistry;
        SharedPtr<MaterialRegistry> m_materialRegistry;
        SharedPtr<PipelineManager> m_pipelineManager;
        SharedPtr<FrameGraph> m_frameGraph;
        SharedPtr<ShaderBundle> m_shaderBundle;

        // Scene Data.
        Model* m_sponzaModelAsset;
        ViewData m_view;
        Camera m_camera;
        FixedArray<PunctualLight, 5> m_lights;
        FixedArray<PunctualLight, 5> m_originalLights;

        // Render Data.
        SharedPtr<RenderMesh> m_sponzaMesh;
        RHIBuffer* m_indirectBuffer;
        size_t m_indirectBufferCount;

        // Assets managing.
        SharedPtr<AssetRegistry> m_assetRegistry;
        SharedPtr<AssetManager> m_assetManager;
    };

}// namespace Wl
