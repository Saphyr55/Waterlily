#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Engine/EngineUpdater.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Scene/Camera.hpp"

namespace Wl
{
    class RenderService;

    // TODO: Those paths must be in function of the project folder. In the future, we should have a builtin engine path (ex. "builtin://Assets/.../GBuffer.hlsl").
    static const StringID GBufferShaderAssetURI = WL_SID("../../../Assets/Shaders/GBuffer.hlsl");
    static const StringID ForwardShaderAssetURI = WL_SID("../../../Assets/Shaders/Forward.hlsl");
    static const StringID LightingShaderAssetPath = WL_SID("../../../Assets/Shaders/Lighting.hlsl");

    static const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

    inline const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    inline const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");
    inline const StringID LightingVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.vert.wlca");
    inline const StringID LightingFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.frag.wlca");

    inline const StringID LudoName = WL_SID("LudoSystem");

    class LUDO_API LudoUpdater : public EngineUpdater
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnTick(double deltaTime) override;

        static inline Camera CreateCamera()
        {
            return Camera::Create(Vector3f(-6.5f, 0.75f, 0.5f), Vector3f(0.0f, 1.0f, 0.0f), 10.0f);
        }

        // Todo: This should be done in dev mode not in runtime mode.
        static inline bool CompileShaders()
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

        LudoUpdater(const SharedPtr<RenderService>& renderService, const SharedPtr<AssetManager>& assetManager)
            : m_renderService(renderService)
            , m_assetManager(assetManager)
        {
        }
        virtual ~LudoUpdater() = default;

    private:
        SharedPtr<RenderService> m_renderService;
        SharedPtr<AssetManager> m_assetManager;

        // Scene Data.
        EntityRegistry m_entityRegistry;
        Camera m_camera;

        // Render Data.
        SharedPtr<RenderMesh> m_sponzaMesh = nullptr;
        RHIBuffer* m_indirectBuffer = nullptr;
        size_t m_indirectBufferCount = 0;
    };

}// namespace Wl
