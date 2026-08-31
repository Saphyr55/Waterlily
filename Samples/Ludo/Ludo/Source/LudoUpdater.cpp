#include "LudoUpdater.hpp"
#include "LightSystem.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Passes/GBufferPass.hpp"
#include "Waterlily/Renderer/Passes/LightingPass.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RenderService.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Shader/ShaderBundle.hpp"
#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Renderer/View.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PointLight.hpp"
#include "Waterlily/Scene/SceneComponent.hpp"

namespace Wl
{

    void LudoUpdater::OnStartup()
    {
        SharedPtr<ShaderBundle> shaderBundle = m_renderService->GetShaderBundle();
        SharedPtr<RHIDevice> device = m_renderService->GetDevice();
        SharedPtr<TextureRegistry> textureRegistry = m_renderService->GetTextureRegistry();
        SharedPtr<MaterialRegistry> materialRegistry = m_renderService->GetMaterialRegistry();

        // TODO: This should be not done in application mode, but only in dev mode.
        WL_CHECK_MSG(CompileShaders(), "Failed to compile shaders.");

        shaderBundle->RegisterGraphicsPass(GBufferPassName, GBufferVertexShaderAssetURI, GBufferFragmentShaderAssetURI);
        shaderBundle->RegisterGraphicsPass(LightingPassName, LightingVertexShaderAssetURI, LightingFragmentShaderAssetURI);
        shaderBundle->LoadAssets();

        m_sponzaModelAsset = m_assetManager->GetAsset<Model>(SponzaModelAssetURI);
        WL_CHECK_MSG(m_sponzaModelAsset, "Failed to load \"%s\" asset.", SponzaModelAssetURI.GetText().GetData());

        Array<StaticMesh*> modelStaticMeshesAsset;
        modelStaticMeshesAsset.Reserve(m_sponzaModelAsset->Meshes.GetSize());

        for (const AssetHandle& meshAssetHandle: m_sponzaModelAsset->Meshes)
        {
            StaticMesh* staticMesh = m_assetManager->GetAsset<StaticMesh>(meshAssetHandle);
            modelStaticMeshesAsset.Append(staticMesh);
        }

        UploadSchedulerInitInfo uploadSchedulerInit = {};
        uploadSchedulerInit.Device = device;
        uploadSchedulerInit.StagingSize = 16 * WL_MB;
        uploadSchedulerInit.MinAlignment = device->GetDeviceProperties().NonCoherentAtomSize;

        UploadScheduler uploadScheduler;
        uploadScheduler.Init(uploadSchedulerInit);

        m_sponzaMesh = MakeShared<RenderMesh>(device);
        // TODO: We should create one big mesh to send.
        m_sponzaMesh->Instanciate(uploadScheduler, m_assetManager, textureRegistry, materialRegistry, modelStaticMeshesAsset[0]);

        for (RenderSubMesh& subMesh: m_sponzaMesh->GetSubMeshes())
        {
            subMesh.Model = Matrix4f::Scale(subMesh.Model, Vector3f(0.005f));
        }

        Array<RHIDrawIndexedCommand> drawIndexedCommands = m_sponzaMesh->CreateDrawIndexedCommands();
        m_indirectBuffer = device->CreateIndirectBuffer<RHIDrawIndexedCommand>(drawIndexedCommands);
        m_indirectBufferCount = drawIndexedCommands.GetSize();

        uploadScheduler.Upload(ArrayView(drawIndexedCommands), m_indirectBuffer);

        textureRegistry->Upload();
        materialRegistry->Upload(device->GetGraphicsQueue());

        device->ImediateSubmit([&uploadScheduler](RHICommandBuffer* commandBuffer) mutable
        {
            double byte = static_cast<double>(uploadScheduler.GetTotalPendingBytes());
            double megaByte = byte / static_cast<double>(WL_MB);
            uploadScheduler.Flush(commandBuffer);
            WL_LOG_DEBUG("Ludo", "Flushed global upload scheduler, total uploaded: %.2lfMB", megaByte);
        }, device->GetGraphicsQueue());

        uploadScheduler.Shutdown();

        textureRegistry->UpdateSRG();
        materialRegistry->UpdateSRG();

        m_camera = CreateCamera();

        m_directionalLight = DirectionalLight {
                .Direction = Vector3f(0.1f, 0.1f, 0.1f),
                .Color = Vector3f(1.0f, 0.90f, 0.75f),
        };

        RegisterLights(m_entityRegistry);

        Input::OnKeyRelease.Connect([this, device, shaderBundle](VirtualKey key) mutable
        {
            if (key == VirtualKey::F2)
            {
                device->WaitIdle();
                if (CompileShaders())
                {
                    shaderBundle->ReloadAssets();
                }
                else
                {
                    WL_LOG_ERROR("Ludo", "Failed to compile shaders.");
                }
            }

            if (key == VirtualKey::F3)
            {
                m_camera.LogDebug();
            }

            if (key == VirtualKey::F4)
            {
                m_camera = CreateCamera();
            }

            if (key == VirtualKey::Escape)
            {
                Engine::GetInstance().RequestStop();
            }
        });

        Input::OnMouseMove.Connect([this](const MouseMove& mouseMove) mutable
        {
            if (Input::ButtonIsDown(Button::Left))
            {
                m_camera.LookAround(mouseMove.PosRelX, -mouseMove.PosRelY);
            }
        });

        m_renderService->GetWindow()->Show();

        // This must be done after filled all SRG layout in the cache.
        m_renderService->GetFrameContext()->InitSRGPools();
    }

    void LudoUpdater::OnTick(double deltaTime)
    {
        SharedPtr<FrameContext> frameContext = m_renderService->GetFrameContext();
        SharedPtr<FrameGraph> frameGraph = m_renderService->GetFrameGraph();
        SharedPtr<ShaderBundle> shaderBundle = m_renderService->GetShaderBundle();

        float aspectRatio = frameContext->GetAspectRatio();

        float width = static_cast<float>(frameContext->GetWidth());
        float height = static_cast<float>(frameContext->GetHeight());

        UpdateLights(m_entityRegistry, deltaTime);

        Vector3f direction(0.0f, 0.0f, 0.0f);

        if (Input::KeyIsDown(VirtualKey::Z))
        {
            direction += m_camera.Front;
        }

        if (Input::KeyIsDown(VirtualKey::S))
        {
            direction -= m_camera.Front;
        }

        if (Input::KeyIsDown(VirtualKey::Q))
        {
            direction -= m_camera.Right;
        }

        if (Input::KeyIsDown(VirtualKey::D))
        {
            direction += m_camera.Right;
        }

        if (Input::KeyIsDown(VirtualKey::Space))
        {
            direction += m_camera.WorldUp;
        }

        if (Input::KeyIsDown(VirtualKey::LeftShift))
        {
            direction -= m_camera.WorldUp;
        }

        if (Vector3f::Length(direction) > 0.0f)
        {
            direction = Vector3f::Normalize(direction);
            m_camera.Position += direction * m_camera.MovementSpeed * static_cast<float>(deltaTime);
            m_camera.UpdateVectors();
        }

        m_camera.UpdateView();

        m_view.View = m_camera.View;
        m_view.Proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
        m_view.ViewProj = m_view.Proj * m_view.View;
        m_view.Eye = m_camera.Position;

        WindowProperties properties = m_renderService->GetWindow()->GetProperties();
        String format = Format("%0.8lf", deltaTime);
        properties.Title = format;
        m_renderService->GetWindow()->SetProperties(properties);

        FrameResult result = frameContext->BeginFrame();
        WL_CHECK(result == FrameResult::Success);

        frameGraph->BeginFrame();

        Frame& frame = frameContext->GetCurrentFrame();

        FramePacketManager packetManager;

        FramePacket packet = packetManager.ExtractPacket(m_view, m_sponzaMesh, m_indirectBufferCount);

        packetManager.PrepareFrame(packet, frame);

        // Light allocation
        EntityView<TransformComponent, LightComponent> lightView = m_entityRegistry.View<TransformComponent, LightComponent>();
        RenderAllocation pointLightsAllocation = frame.UniformAllocator.AllocateArray<PointLight>(lightView.GetSize());

        size_t i = 0;
        for (const auto [entity, transform, light]: lightView)
        {
            pointLightsAllocation.Get<PointLight>()[i++] = PointLight(transform.Position, light.Color);
        }

        RenderAllocation directionalLightAllocation = frame.UniformAllocator.Allocate<DirectionalLight>();
        frame.UniformAllocator.UpdateData(directionalLightAllocation, m_directionalLight);

        RenderAllocation countersAllocation = frame.UniformAllocator.Allocate<uint32_t>();
        countersAllocation.Update<uint32_t>(lightView.GetSize());

        Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        Rect2D scissor(0.0f, 0.0f, width, height);

        PassContext passContext = {};
        passContext.FrameGraph = frameGraph;
        passContext.MaterialRegistry = m_renderService->GetMaterialRegistry();
        passContext.TextureRegistry = m_renderService->GetTextureRegistry();
        passContext.PipelineManager = m_renderService->GetPipelineManager();

        FrameGraphBufferHandle indirect = frameGraph->ImportBuffer(m_indirectBuffer, m_indirectBuffer->GetSize(), 0);

        FrameGraphTextureInfo colorTextureInfo = {};
        colorTextureInfo.Name = "Color";
        colorTextureInfo.Format = frameContext->GetSwapchain()->GetFormat();
        colorTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle color = frameGraph->CreateTexture(colorTextureInfo);

        FrameGraphTextureInfo positionTextureInfo = {};
        positionTextureInfo.Name = "Position";
        positionTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        positionTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle position = frameGraph->CreateTexture(positionTextureInfo);

        FrameGraphTextureInfo normalTextureInfo = {};
        normalTextureInfo.Name = "Normal";
        normalTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        normalTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle normal = frameGraph->CreateTexture(normalTextureInfo);

        FrameGraphTextureInfo albedoTextureInfo = {};
        albedoTextureInfo.Name = "Albedo";
        albedoTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        albedoTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle albedo = frameGraph->CreateTexture(albedoTextureInfo);

        FrameGraphTextureInfo depthStencilTextureInfo = {};
        depthStencilTextureInfo.Name = "DepthScentil";
        depthStencilTextureInfo.Format = RHIFormat::D24S8;
        depthStencilTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle depthStencil = frameGraph->CreateTexture(depthStencilTextureInfo);

        GBufferPassParameters gBufferParams = {};
        gBufferParams.Position = position;
        gBufferParams.Normal = normal;
        gBufferParams.Albedo = albedo;
        gBufferParams.DepthStencil = depthStencil;
        gBufferParams.Indirect = indirect;

        ShaderGraphicsPass& shaderGBufferPass = shaderBundle->GetShaderGraphicsPass(GBufferPassName);
        shaderGBufferPass.PipelineState.CullMode = RHICullModeFlags::Back;
        shaderGBufferPass.PipelineState.Viewport = viewport;
        shaderGBufferPass.PipelineState.Scissor = scissor;

        FrameGraphPass& gBufferPass = GBufferPassCreate(passContext, packet, shaderGBufferPass.PipelineState, gBufferParams);

        LightingPassParameters lightingParams = {};
        lightingParams.Color = color;
        lightingParams.Albedo = albedo;
        lightingParams.Normal = normal;
        lightingParams.Position = position;
        lightingParams.Indirect = indirect;
        lightingParams.DepthStencil = depthStencil;
        lightingParams.PointLightsAllocation = &pointLightsAllocation;
        lightingParams.DirectionalLightAllocation = &directionalLightAllocation;
        lightingParams.CountersAllocation = &countersAllocation;

        ShaderGraphicsPass& shaderLightingPass = shaderBundle->GetShaderGraphicsPass(LightingPassName);
        shaderLightingPass.PipelineState.CullMode = RHICullModeFlags::None;
        shaderLightingPass.PipelineState.Viewport = viewport;
        shaderLightingPass.PipelineState.Scissor = scissor;

        FrameGraphPass& lightingPass = LightingPassCreate(passContext, packet, shaderLightingPass.PipelineState, lightingParams);

        frameGraph->AddOutput(color);
        frameGraph->Compile();

        m_renderService->GetOrCreatePipeline(gBufferPass, shaderGBufferPass.PipelineState);
        m_renderService->GetOrCreatePipeline(lightingPass, shaderLightingPass.PipelineState);

        frameGraph->Execute(frame.CommandBuffer);
        frameGraph->EndFrame();
        frameContext->EndFrame();
    }

    void LudoUpdater::OnShutdown()
    {
        m_sponzaMesh->Destroy();

        m_renderService->GetDevice()->DestroyBuffer(m_indirectBuffer);

        m_renderService->GetWindow()->Close();
    }

}// namespace Wl
