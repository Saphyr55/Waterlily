#include "LudoApplication.hpp"

#include "LightSystem.hpp"
#include "Passes/GBufferPass.hpp"
#include "Passes/LightingPass.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FramePacket.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/View.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PointLight.hpp"
#include "Waterlily/Scene/SceneComponent.hpp"


namespace Wl
{

    void LudoApplicationDelegate::OnStartup()
    {
        m_window = Window::Create(WindowProperties("Demo Window", 1080, 720, 100, 100));

        FileSystem& assetsFileSystem = FileSystem::GetPlatform();

        m_assetRegistry = AssetRegistry::LoadDefault(assetsFileSystem);

        SharedPtr<AssetLoader> assetLoader = MakeShared<ConditionnedAssetLoader>(assetsFileSystem);
        m_assetManager = MakeShared<AssetManager>(m_assetRegistry, assetLoader);

        m_device = RHIDeviceFactory::Create(RHIGraphicsAPI::Vulkan);
        m_device->Init(m_window->GetNativeWindow());

        FrameContextInitInfo frameContextInitInfo = {};
        frameContextInitInfo.FrameAllocationSize = 16 * WL_MB;
        frameContextInitInfo.StagingBufferSize = 16 * WL_MB;
        frameContextInitInfo.StorageBufferSize = 16 * WL_MB;
        frameContextInitInfo.UniformBufferSize = 16 * WL_MB;
        frameContextInitInfo.FrameWidth = m_window->GetProperties().Width;
        frameContextInitInfo.FrameHeight = m_window->GetProperties().Height;

        m_frameContext = MakeShared<FrameContext>( m_device);
        m_frameContext->Init(frameContextInitInfo);

        m_textureRegistry = MakeShared<TextureRegistry>(m_device, *m_assetManager, SRGBindingTextures);
        m_materialRegistry = MakeShared<MaterialRegistry>(m_device, SRGBindingMaterials);

        m_pipelineManager = MakeShared<PipelineManager>(m_device, m_frameContext->GetSRGLayoutCache(), assetsFileSystem);
        m_shaderBundle = MakeShared<ShaderBundle>(m_assetRegistry, m_assetManager, m_pipelineManager);

        // TODO: This should be not done in application mode, but only in dev mode.
        WL_CHECK_MSG(CompileShaders(), "Failed to compile shaders.");
        m_shaderBundle->RegisterGraphicsPass(GBufferPassName, GBufferVertexShaderAssetURI, GBufferFragmentShaderAssetURI);
        m_shaderBundle->RegisterGraphicsPass(LightingPassName, LightingVertexShaderAssetURI, LightingFragmentShaderAssetURI);
        m_shaderBundle->LoadAssets();

        m_sponzaModelAsset = m_assetManager->GetAsset<Model>(SponzaModelAssetURI);
        WL_CHECK_MSG(m_sponzaModelAsset, "Failed to load \"%s\" asset.", SponzaModelAssetURI.GetText().GetData());

        Array<StaticMesh*> modelStaticMeshesAsset;
        modelStaticMeshesAsset.Reserve(m_sponzaModelAsset->Meshes.GetSize());

        for (const AssetHandle& meshAssetHandle: m_sponzaModelAsset->Meshes)
        {
            modelStaticMeshesAsset.Append(m_assetManager->GetAsset<StaticMesh>(meshAssetHandle));
        }

        UploadSchedulerInitInfo uploadSchedulerInit = {};
        uploadSchedulerInit.Device = m_device;
        uploadSchedulerInit.StagingSize = 16 * WL_MB;
        uploadSchedulerInit.MinAlignment = m_device->GetDeviceProperties().NonCoherentAtomSize;

        UploadScheduler uploadScheduler;
        uploadScheduler.Init(uploadSchedulerInit);

        m_sponzaMesh = MakeShared<RenderMesh>(m_device);
        // TODO: We should create one big mesh to send.
        m_sponzaMesh->Instanciate(uploadScheduler, m_assetManager, m_textureRegistry, m_materialRegistry, modelStaticMeshesAsset[0]);

        for (RenderSubMesh& subMesh: m_sponzaMesh->GetSubMeshes())
        {
            subMesh.Model = Matrix4f::Scale(subMesh.Model, Vector3f(0.005f));
        }

        Array<RHIDrawIndexedCommand> drawIndexedCommands = m_sponzaMesh->CreateDrawIndexedCommands();
        m_indirectBuffer = m_device->CreateIndirectBuffer<RHIDrawIndexedCommand>(drawIndexedCommands);
        m_indirectBufferCount = drawIndexedCommands.GetSize();

        uploadScheduler.Upload(ArrayView(drawIndexedCommands), m_indirectBuffer);

        m_textureRegistry->Upload();
        m_materialRegistry->Upload(m_device->GetGraphicsQueue());

        m_device->ImediateSubmit([&uploadScheduler](RHICommandBuffer* commandBuffere) mutable
        {
            double byte = static_cast<double>(uploadScheduler.GetTotalPendingBytes());
            double megaByte = byte / static_cast<double>(WL_MB);
            uploadScheduler.Flush(commandBuffere);
            WL_LOG_DEBUG("Ludo", "Flushed global upload scheduler, total uploaded: %.2lfMB", megaByte);
        }, m_device->GetGraphicsQueue());

        uploadScheduler.Shutdown();

        m_textureRegistry->UpdateSRG();
        m_materialRegistry->UpdateSRG();

        m_camera = InitCamera();
        m_directionalLight = DirectionalLight {
                .Direction = Vector3f(0.1f, 0.1f, 0.1f),
                .Color = Vector3f(1.0f, 0.90f, 0.75f),
        };
        RegisterLights(m_entityRegistry);

        m_frameGraph = MakeShared<FrameGraph>(m_frameContext);

        m_window->GetEventHandler().OnMinimized.Connect([]()
        {
            Engine::GetInstance().GetApplication()->Pause();
        });

        m_window->GetEventHandler().OnExposed.Connect([]()
        {
            Engine::GetInstance().GetApplication()->Unpause();
        });

        m_window->GetEventHandler().OnClose.Connect([]()
        {
            WL_LOG_INFO("Ludo", "Window closed.");
            Engine::GetInstance().GetApplication()->RequestStop();
        });

        m_window->GetEventHandler().OnResized.Connect([this](uint32_t width, uint32_t height)
        {
            WL_LOG_INFO("Ludo", "Window resized to %dx%d", width, height);
            m_frameContext->Resize(width, height);
            m_frameGraph->Resize();
        });

        Input::OnKeyRelease.Connect([this](VirtualKey key)
        {
            if (key == VirtualKey::F2)
            {
                m_device->WaitIdle();
                if (CompileShaders())
                {
                    m_shaderBundle->ReloadAssets();
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
                m_camera = InitCamera();
            }

            if (key == VirtualKey::Escape)
            {
                Engine::GetInstance().GetApplication()->RequestStop();
            }
        });

        Input::OnMouseMove.Connect([this](const MouseMove& mouseMove)
        {
            if (Input::ButtonIsDown(Button::Left))
            {
                m_camera.LookAround(mouseMove.PosRelX, -mouseMove.PosRelY);
            }
        });

        m_window->Show();

        // This must be done after filled all SRG layout in the cache.
        m_frameContext->InitSRGPools();
    }

    void LudoApplicationDelegate::OnUpdate(double deltaTime)
    {
        float aspectRatio = m_frameContext->GetAspectRatio();

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

        WindowProperties properties = m_window->GetProperties();
        String format = Format("%0.8lf", deltaTime);
        properties.Title = format;
        m_window->SetProperties(properties);
    }

    void LudoApplicationDelegate::OnRender()
    {
        FrameResult result = m_frameContext->BeginFrame();
        WL_CHECK(result == FrameResult::Success);

        m_frameGraph->BeginFrame();

        Frame& frame = m_frameContext->GetCurrentFrame();

        FramePacketManager packetManager;
        FramePacket packet = packetManager.ExtractPacket(m_view, m_sponzaMesh, m_indirectBufferCount);
        packetManager.PrepareFrame(packet, frame);

        // Light allocation
        auto lightView = m_entityRegistry.View<TransformComponent, LightComponent>();
        RenderAllocation pointLightsAllocation = frame.UniformAllocator.AllocateArray<PointLight>(lightView.GetSize());

        size_t i = 0;
        for (const auto& [entity, transform, light]: lightView)
        {
            pointLightsAllocation.Get<PointLight>()[i++] = PointLight(transform.Position, light.Color);
        }

        RenderAllocation directionalLightAllocation = frame.UniformAllocator.Allocate<DirectionalLight>();
        frame.UniformAllocator.UpdateData(directionalLightAllocation, m_directionalLight);

        RenderAllocation countersAllocation = frame.UniformAllocator.Allocate<uint32_t>();
        countersAllocation.Update<uint32_t>(lightView.GetSize());

        PassContext passContext = {};
        passContext.FrameGraph = m_frameGraph;
        passContext.MaterialRegistry = m_materialRegistry;
        passContext.TextureRegistry = m_textureRegistry;
        passContext.PipelineManager = m_pipelineManager;

        FrameGraphBufferHandle indirect = m_frameGraph->ImportBuffer(m_indirectBuffer, m_indirectBuffer->GetSize(), 0);

        FrameGraphTextureInfo colorTextureInfo = {};
        colorTextureInfo.Name = "Color";
        colorTextureInfo.Format = m_frameContext->GetSwapchain()->GetFormat();
        colorTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle color = m_frameGraph->CreateTexture(colorTextureInfo);

        FrameGraphTextureInfo positionTextureInfo = {};
        positionTextureInfo.Name = "Position";
        positionTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        positionTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle position = m_frameGraph->CreateTexture(positionTextureInfo);

        FrameGraphTextureInfo normalTextureInfo = {};
        normalTextureInfo.Name = "Normal";
        normalTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        normalTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle normal = m_frameGraph->CreateTexture(normalTextureInfo);

        FrameGraphTextureInfo albedoTextureInfo = {};
        albedoTextureInfo.Name = "Albedo";
        albedoTextureInfo.Format = RHIFormat::RGBA16_FLOAT;
        albedoTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle albedo = m_frameGraph->CreateTexture(albedoTextureInfo);

        FrameGraphTextureInfo depthStencilTextureInfo = {};
        depthStencilTextureInfo.Name = "DepthScentil";
        depthStencilTextureInfo.Format = RHIFormat::D24S8;
        depthStencilTextureInfo.SizeClass = SizeClass::Swapchain;
        FrameGraphTextureHandle depthStencil = m_frameGraph->CreateTexture(depthStencilTextureInfo);

        GBufferPassParameters gBufferParams = {};
        gBufferParams.Position = position;
        gBufferParams.Normal = normal;
        gBufferParams.Albedo = albedo;
        gBufferParams.DepthStencil = depthStencil;
        gBufferParams.Indirect = indirect;

        ShaderGraphicsPass& shaderGBufferPass = m_shaderBundle->GetShaderGraphicsPass(GBufferPassName);
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

        ShaderGraphicsPass& shaderLightingPass = m_shaderBundle->GetShaderGraphicsPass(LightingPassName);
        FrameGraphPass& lightingPass = LightingPassCreate(passContext, packet, shaderLightingPass.PipelineState, lightingParams);

        m_frameGraph->AddOutput(color);
        m_frameGraph->Compile();

        float width = static_cast<float>(m_frameContext->GetWidth());
        float height = static_cast<float>(m_frameContext->GetHeight());
        Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        Rect2D scissor(0.0f, 0.0f, width, height);

        GetOrCreatePipeline(gBufferPass,
                            shaderGBufferPass.PipelineState,
                            RHICullModeFlags::Back,
                            viewport,
                            scissor);

        GetOrCreatePipeline(lightingPass,
                            shaderLightingPass.PipelineState,
                            RHICullModeFlags::None,
                            viewport,
                            scissor);

        m_frameGraph->Execute(frame.CommandBuffer);
        m_frameGraph->EndFrame();
        m_frameContext->EndFrame();
    }

    RHIPipeline* LudoApplicationDelegate::GetOrCreatePipeline(FrameGraphPass& pass,
                                                              GraphicsPipelineState& state,
                                                              RHICullModeFlags cullMode,
                                                              const Viewport& viewport,
                                                              const Rect2D& scissor)
    {
        state.SRGLayouts[SRGIndexTextures] = m_textureRegistry->GetSRGLayout();
        state.SRGLayouts[SRGIndexMaterials] = m_materialRegistry->GetSRGLayout();
        state.CullMode = cullMode;
        state.Viewport = viewport;
        state.Scissor = scissor;
        state.VertexShader = state.VertexShader;
        state.FragmentShader = state.FragmentShader;
        state.RenderPass = m_frameGraph->GetRenderPass(pass.GetName());

        return m_pipelineManager->GetOrCreate(pass.GetName(), state);
    }

    void LudoApplicationDelegate::OnShutdown()
    {
        m_frameGraph->Destroy();

        m_sponzaMesh->Destroy();

        m_device->DestroyBuffer(m_indirectBuffer);

        m_pipelineManager->Destroy();

        m_materialRegistry->Destroy();
        m_textureRegistry->Destroy();

        m_frameContext->Destroy();
        m_device->Destroy();

        m_window->Close();
    }

}// namespace Wl
