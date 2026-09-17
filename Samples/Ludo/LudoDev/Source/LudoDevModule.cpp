#include "LudoDevModule.hpp"
#include "LudoDevShader.hpp"

#include "LudoAssets.hpp"
#include "LudoModule.hpp"

#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Renderer/RenderService.hpp"


namespace Ludo
{

    WL_REGISTER_MODULE(LudoDevModule, "Ludo.Dev");

    bool LudoDevModule::CompileShaders()
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

    void LudoDevModule::OnStartup()
    {
        WL_LOG_INFO("LudoDev", "Ludo.Dev Module started.");

        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();

        LudoModule* ludoModule = moduleRegistry.GetModule<LudoModule>("Ludo");
        SharedPtr<RenderService> renderService = ludoModule->GetRenderService();

        m_shaderCompiler = IShaderCompiler::Create("../../../Assets/Shaders");

        WL_CHECK_MSG(CompileShaders(), "Failed to compile shaders.");
        
        Input::OnKeyRelease.Connect([=, this](VirtualKey key) mutable
        {
            if (key == VirtualKey::F2)
            {
                bool isCompileShaderSuccessed = CompileShaders();

                if (isCompileShaderSuccessed)
                {   
                    renderService->GetDevice()->WaitIdle();
                    renderService->GetShaderBundle()->ReloadAssets();
                }
                else
                {
                    WL_LOG_ERROR("Ludo", "Failed to compile shaders.");
                }
            }
        });
    }

    void LudoDevModule::OnShutdown()
    {
        WL_LOG_INFO("LudoDev", "Ludo.Dev Module stopped.");
    }

}// namespace Ludo