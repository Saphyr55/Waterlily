#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Assets/AssetManager.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Engine/EngineSubSystem.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderService.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Scene/Camera.hpp"

namespace Wl
{
    // TODO: Those paths must be in function of the project folder. In the future, we should have a builtin engine path (ex. "builtin://Assets/.../GBuffer.slang").
    static const StringID GBufferShaderAssetURI = WL_SID("../../../Assets/Shaders/Pass/GBuffer.slang");
    static const StringID ForwardShaderAssetURI = WL_SID("../../../Assets/Shaders/Pass/Forward.slang");
    static const StringID LightingShaderAssetURI = WL_SID("../../../Assets/Shaders/Pass/Lighting.slang");
    static const StringID ShadowMapShaderAssetURI = WL_SID("../../../Assets/Shaders/Pass/CSM.slang");

    static const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

    inline const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    inline const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");

    inline const StringID ShadowMapVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/ShadowMap.vert.wlca");
    inline const StringID ShadowMapFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/ShadowMap.frag.wlca");

    inline const StringID LightingComputeShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.comp.wlca");

    inline const StringID LudoName = WL_SID("LudoSystem");

    class LUDO_API LudoSubSystem : public EngineSubSystem
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnTick(double deltaTime) override;

        static inline Camera CreateCamera()
        {
            return Camera::Create(Vector3f(-6.0f, 1.0f, -0.1f), Vector3f(-15.0f, 1.0f, 0.0f), 6.0f);
        }

        // Todo: This should be done in dev mode not in runtime mode.
        inline bool CompileShaders()
        {
            bool success = m_shaderCompiler->Compile({
                                   GBufferShaderAssetURI.GetText(),
                                   GBufferVertexShaderAssetURI.GetText(),
                                   "VSMain",
                                   Shader::Stage::Vertex,
                           }) == ShaderCompileResult::Success;

            success = success && m_shaderCompiler->Compile({
                                         GBufferShaderAssetURI.GetText(),
                                         GBufferFragmentShaderAssetURI.GetText(),
                                         "FSMain",
                                         Shader::Stage::Fragment,
                                 }) == ShaderCompileResult::Success;

            success = success && m_shaderCompiler->Compile({
                                         ShadowMapShaderAssetURI.GetText(),
                                         ShadowMapVertexShaderAssetURI.GetText(),
                                         "VSMain",
                                         Shader::Stage::Vertex,
                                 }) == ShaderCompileResult::Success;

            success = success && m_shaderCompiler->Compile({
                                         ShadowMapShaderAssetURI.GetText(),
                                         ShadowMapFragmentShaderAssetURI.GetText(),
                                         "FSMain",
                                         Shader::Stage::Fragment,
                                 }) == ShaderCompileResult::Success;

            success = success && m_shaderCompiler->Compile({
                                         LightingShaderAssetURI.GetText(),
                                         LightingComputeShaderAssetURI.GetText(),
                                         "Main",
                                         Shader::Stage::Compute,
                                 }) == ShaderCompileResult::Success;

            return success;
        }

        LudoSubSystem(const SharedPtr<RenderService>& renderService,
                      const SharedPtr<AssetManager>& assetManager)
            : m_renderService(renderService)
            , m_assetManager(assetManager)
            // TODO: This should be done in dev mode not in runtime mode.
            , m_shaderCompiler(IShaderCompiler::Create("../../../Assets/Shaders/"))
        {
        }
        virtual ~LudoSubSystem() = default;

    private:
        SharedPtr<IShaderCompiler> m_shaderCompiler;

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
