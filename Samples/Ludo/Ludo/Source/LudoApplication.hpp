#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/Engine/ApplicationDelegate.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Pipeline.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Shader/ShaderBundle.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Renderer/View.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PointLight.hpp"

namespace Wl
{

    // TODO: Those paths must be in function of the project folder. In the future, we should have a builtin engine path (ex. "builtin://Assets/.../GBuffer.hlsl").
    static const StringID GBufferShaderAssetURI = WL_SID("../../../Assets/Shaders/GBuffer.hlsl");
    static const StringID ForwardShaderAssetURI = WL_SID("../../../Assets/Shaders/Forward.hlsl");
    static const StringID LightingShaderAssetPath = WL_SID("../../../Assets/Shaders/Lighting.hlsl");

    static const StringID AssetRegistryURI = WL_SID("Assets/Registry.wlar");
    static const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

    inline const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    inline const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");
    inline const StringID LightingVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.vert.wlca");
    inline const StringID LightingFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.frag.wlca");

    class LUDO_API LudoApplicationDelegate : public ApplicationDelegate
    {
    public:
        virtual void OnStartup() override;

        virtual void OnRender() override;

        virtual void OnUpdate(double deltaTime) override;

        virtual void OnShutdown() override;

    public:
        LudoApplicationDelegate() = default;
        ~LudoApplicationDelegate() = default;

    private:
        RHIPipeline* GetOrCreatePipeline(FrameGraphPass& pass,
                                         GraphicsPipelineState& state,
                                         RHICullModeFlags cullMode,
                                         const Viewport& viewport,
                                         const Rect2D& scissor);

    private:
        // Rendering.
        SharedPtr<Window> m_window = nullptr;
        SharedPtr<RHIDevice> m_device = nullptr;
        SharedPtr<FrameContext> m_frameContext = nullptr;

        // Graphics.
        SharedPtr<TextureRegistry> m_textureRegistry = nullptr;
        SharedPtr<MaterialRegistry> m_materialRegistry = nullptr;
        SharedPtr<PipelineManager> m_pipelineManager = nullptr;
        SharedPtr<FrameGraph> m_frameGraph = nullptr;
        SharedPtr<ShaderBundle> m_shaderBundle = nullptr;

        // Scene Data.
        EntityRegistry m_entityRegistry;
        Model* m_sponzaModelAsset = nullptr;
        ViewData m_view;
        Camera m_camera;
        DirectionalLight m_directionalLight;

        // Render Data.
        SharedPtr<RenderMesh> m_sponzaMesh = nullptr;
        RHIBuffer* m_indirectBuffer = nullptr;
        size_t m_indirectBufferCount = 0;

        // Assets managing.
        SharedPtr<AssetRegistry> m_assetRegistry = nullptr;
        SharedPtr<AssetManager> m_assetManager = nullptr;
    };

    inline Camera InitCamera()
    {
        Camera camera(Vector3f(-6.5f, 0.75f, 0.5f));
        camera.MovementSpeed = 10.0f;
        camera.LookAt(Vector3f(0.0f, 1.0f, 0.0f));
        camera.UpdateView();
        camera.UpdateVectors();
        return camera;
    }

    // Todo: This should be done in dev mode not in runtime mode.
    inline bool CompileShaders()
    {
        FileSystem& fileSystem = FileSystem::GetPlatform();

        bool success = SPIRVShaderCompiler::CompileHLSL(GBufferShaderAssetURI.GetText(),
                                                        GBufferVertexShaderAssetURI.GetText(),
                                                        "VSMain",
                                                        Shader::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(GBufferShaderAssetURI.GetText(),
                                                              GBufferFragmentShaderAssetURI.GetText(),
                                                              "FSMain",
                                                              Shader::Stage::Fragment);

        success = success && SPIRVShaderCompiler::CompileHLSL(LightingShaderAssetPath.GetText(),
                                                              LightingVertexShaderAssetURI.GetText(),
                                                              "VSMain",
                                                              Shader::Stage::Vertex);

        success = success && SPIRVShaderCompiler::CompileHLSL(LightingShaderAssetPath.GetText(),
                                                              LightingFragmentShaderAssetURI.GetText(),
                                                              "FSMain",
                                                              Shader::Stage::Fragment);
        return success;
    }

}// namespace Wl
