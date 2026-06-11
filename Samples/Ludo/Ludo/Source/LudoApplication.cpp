#include "LudoApplication.hpp"

#include "LudoTypes.hpp"
#include "Passes/GBufferPass.hpp"
#include "Passes/LightingPass.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/Mesh/RenderMesh.hpp"
#include "Waterlily/Renderer/Mesh/StaticMesh.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RenderView.hpp"
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"
#include "Waterlily/Renderer/Shader/ShaderCompiler.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PunctualLight.hpp"


namespace Wl
{
    static const StringID AssetRegistryURI = WL_SID("Assets/Registry.wlar");
    static const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

    static const StringID GBufferVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.vert.wlca");
    static const StringID GBufferFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/GBuffer.frag.wlca");
    static const StringID LightingVertexShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.vert.wlca");
    static const StringID LightingFragmentShaderAssetURI = WL_SID("Assets/Shaders/SPV/Lighting.frag.wlca");

    static bool CompileShaders()
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

    static FixedArray<PunctualLight, 5> MakeLights()
    {
        FixedArray<PunctualLight, 5> lights;

        lights[0] = {
                {3.49f, 3.49f, -2.34f},
                {0.1f,  0.6f,  0.9f  },
        };

        lights[1] = {
                {-5.76f, 1.22f, -0.82f},
                {1.0f,   0.6f,  0.1f  },
        };

        lights[2] = {
                {-0.46f, 3.63f, 1.88f},
                {0.4f,   1.0f,  0.2f },
        };

        lights[3] = {
                {-2.07f, 1.94f, 0.04f},
                {0.0f,   0.5f,  1.0f },
        };

        lights[4] = {
                {-6.5f, 0.75f, 0.5f},
                {0.4f,  0.8f,  0.8f},
        };

        return lights;
    }

    static Camera InitCamera()
    {
        Camera camera(Vector3f(-6.5f, 0.75f, 0.5f));
        camera.MovementSpeed = 10.0f;
        camera.LookAt(Vector3f(0.0f, 1.0f, 0.0f));
        camera.UpdateView();
        camera.UpdateVectors();
        return camera;
    }

    int32_t LudoApplication(Application& application)
    {
        WindowProperties windowProperties("Demo Window", 1080, 720, 100, 100);
        SharedPtr<Window> window = Window::Create(windowProperties);

        SharedPtr<RHIDevice> device = RHIDeviceFactory::Create(RHIGraphicsAPI::Vulkan);
        device->Init(window->GetNativeWindow());

        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();

        Camera initialCamera = InitCamera();
        Camera camera = initialCamera;

        Input::OnMouseMove.Connect([&](const MouseMove& mouseMove) -> void
        {
            if (Input::ButtonIsDown(Button::Left))
            {
                camera.LookAround(mouseMove.PosRelX, -mouseMove.PosRelY);
            }
        });

        FileSystem& assetsFileSystem = FileSystem::GetPlatform();

        FileResult assetRegistryFileResult = assetsFileSystem.OpenRead(AssetRegistryURI.GetText());
        WL_CHECK_MSG(assetRegistryFileResult.HasValue(), "Impossible to read \"%s\"", AssetRegistryURI.GetText().GetData());
        File& fileAssetRegistry = *assetRegistryFileResult.GetValue();

        SharedPtr<AssetRegistry> assetRegistry = AssetRegistry::LoadFromFile(fileAssetRegistry);
        WL_CHECK(assetRegistry);
        fileAssetRegistry.Close();

        SharedPtr<IAssetLoader> assetLoader = MakeShared<ConditionnedAssetLoader>(assetsFileSystem);
        SharedPtr<AssetManager> assetManager = MakeShared<AssetManager>(assetRegistry, assetLoader);

        SharedPtr<FrameContext> frameContext = MakeShared<FrameContext>();
        FrameContextInitInfo frameContextInitInfo = {};
        frameContextInitInfo.StagingBufferSize = 16 * WL_MB;
        frameContextInitInfo.StorageBufferSize = 16 * WL_MB;
        frameContextInitInfo.UniformBufferSize = 16 * WL_MB;
        frameContextInitInfo.FrameWidth = window->GetProperties().Width;
        frameContextInitInfo.FrameHeight = window->GetProperties().Height;
        frameContext->Init(device, frameContextInitInfo);

        SharedPtr<TextureRegistry> textureRegistry = MakeShared<TextureRegistry>(device, *assetManager, LudoTexturesSRGBinding);
        SharedPtr<MaterialRegistry> materialRegistry = MakeShared<MaterialRegistry>(device, LudoMaterialsSRGBinding);
        SharedPtr<PipelineManager> pipelineManager = MakeShared<PipelineManager>(device, frameContext->GetSRGLayoutCache(), assetsFileSystem);

        // This should be not done in application mode, but only in dev mode.
        bool isCompilingShaderSuccessed = Wl::CompileShaders();
        WL_CHECK_MSG(isCompilingShaderSuccessed, "Failed to compile shaders.");

        AssetHandle gBufferVertexShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, GBufferVertexShaderAssetURI);
        AssetHandle gBufferFragmentShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, GBufferFragmentShaderAssetURI);
        AssetHandle lightingVertexShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, LightingVertexShaderAssetURI);

        AssetHandle lightingFragmentShaderAssetHandle = assetRegistry->CreateAsset(AssetType_Shader, LightingFragmentShaderAssetURI);

        Shader* gBufferVertexShaderAsset;
        Shader* gBufferFragmentShaderAsset;
        Shader* lightingVertexShaderAsset;
        Shader* lightingFragmentShaderAsset;

        auto loadAssets = [&](bool reload = false)
        {
            gBufferVertexShaderAsset = assetManager->GetAsset<Shader>(gBufferVertexShaderAssetHandle, reload);
            gBufferFragmentShaderAsset = assetManager->GetAsset<Shader>(gBufferFragmentShaderAssetHandle, reload);
            lightingVertexShaderAsset = assetManager->GetAsset<Shader>(lightingVertexShaderAssetHandle, reload);
            lightingFragmentShaderAsset = assetManager->GetAsset<Shader>(lightingFragmentShaderAssetHandle, reload);
        };

        loadAssets();

        Model* sponzaModelAsset = assetManager->GetAsset<Model>(SponzaModelAssetURI);
        WL_CHECK_MSG(sponzaModelAsset, "Impossible to load \"%s\" asset.", SponzaModelAssetURI.GetText().GetData());

        Array<StaticMesh*> modelStaticMeshesAsset(sponzaModelAsset->Meshes.GetSize());
        for (const AssetHandle& meshAssetHandle: sponzaModelAsset->Meshes)
        {
            modelStaticMeshesAsset.Append(assetManager->GetAsset<StaticMesh>(meshAssetHandle));
        }

        UploadScheduler uploadScheduler;
        UploadSchedulerInitInfo uploadSchedulerInit = {};
        uploadSchedulerInit.Device = device;
        uploadSchedulerInit.StagingSize = 16 * WL_MB;
        uploadSchedulerInit.MinAlignment = device->GetDeviceProperties().NonCoherentAtomSize;
        uploadScheduler.Init(uploadSchedulerInit);

        UIDrawElement uiDrawElement;
        RenderMesh uiMesh = uiDrawElement.Instanciate(device, uploadScheduler);

        RenderMesh sponzaMesh(device);
        // TODO: We should create one big mesh to send.
        sponzaMesh.Instanciate(uploadScheduler, assetManager, textureRegistry, materialRegistry, modelStaticMeshesAsset[0]);

        for (RenderSubMesh& subMesh: sponzaMesh.GetSubMeshes())
        {
            subMesh.Model = Matrix4f::Scale(subMesh.Model, Vector3f(0.005f));
        }

        Array<RHIDrawIndexedCommand> drawIndexedCommands = sponzaMesh.CreateDrawIndexedCommands();
        RHIBuffer* indirectBuffer = device->CreateIndirectBuffer(drawIndexedCommands);

        uploadScheduler.Upload(ArrayView(drawIndexedCommands), indirectBuffer);
        textureRegistry->Upload();
        materialRegistry->Upload(device->GetGraphicsQueue());

        device->ImediateSubmit([&](RHICommandBuffer* commandBuffere)
        {
            double byte = static_cast<double>(uploadScheduler.GetTotalPendingBytes());
            double megaByte = byte / static_cast<double>(WL_MB);
            uploadScheduler.Flush(commandBuffere);
            WL_LOG_DEBUG("Ludo", "Flushed global upload scheduler, total uploaded: %.2lfMB", megaByte);
        }, device->GetGraphicsQueue());

        uploadScheduler.Shutdown();

        textureRegistry->CompileSRG();
        materialRegistry->CompileShaderResource();

        frameContext->InitializeSRGPools();

        FixedArray<PunctualLight, 5> originalsPunctualLights = MakeLights();
        FixedArray<PunctualLight, 5> punctualLights = MakeLights();

        SharedPtr<FrameGraph> frameGraph = MakeShared<FrameGraph>(frameContext);

        window->GetEventHandler().OnClose.Connect([&]()
        {
            WL_LOG_INFO("Ludo", "Window closed.");
            application.Stop();
        });

        window->GetEventHandler().OnResized.Connect([&](uint32_t width, uint32_t height)
        {
            WL_LOG_INFO("Ludo", "Window resized to %dx%d", width, height);
            frameContext->Resize(width, height);
            frameGraph->Resize();
        });

        GraphicsPipelineState gbufferPipelineProperties = {};
        GraphicsPipelineState lightingPipelineProperties = {};

        auto getOrCreatePipeline = [&](FrameGraphPass& pass,
                                       GraphicsPipelineState& state,
                                       RHICullModeFlags cullMode,
                                       const Viewport& viewport,
                                       const Rect2D& scissor,
                                       Shader* vertexShader,
                                       Shader* fragmentShader) -> RHIGraphicsPipeline*
        {
            state.SRGLayouts[LudoTextureGRGIndex] = textureRegistry->GetSRGLayout();
            state.SRGLayouts[LudoMaterialsSRGIndex] = materialRegistry->GetSRGLayout();
            state.CullMode = cullMode;
            state.Viewport = viewport;
            state.Scissor = scissor;
            state.VertexShader = vertexShader;
            state.FragmentShader = fragmentShader;
            state.RenderPass = frameGraph->GetRenderPass(pass.GetName());

            return pipelineManager->GetOrCreate(pass.GetName(), state);
        };

        Input::OnKeyRelease.Connect([&](VirtualKey key)
        {
            switch (key)
            {
                case VirtualKey::F3:
                {
                    HashMap<StringID, GraphicsPipelineState*> propsMap = {
                            {GBufferPassName,  &gbufferPipelineProperties },
                            {LightingPassName, &lightingPipelineProperties}
                    };
                    device->WaitIdle();

                    CompileShaders();

                    loadAssets(true);

                    for (auto [name, props]: propsMap)
                    {
                        pipelineManager->Recreate(name, *props);
                    }
                    break;
                }
                case VirtualKey::F2:
                {
                    camera.LogDebug();
                    break;
                }
                default:
                    break;
            }
        });

        RenderView view;

        float lightVelocity = 3.1f;
        float lightThresholdPosition = 3.0f;

        // Update handle
        application.OnTick.Connect([&](Timer& timer)
        {
            float deltaTime = static_cast<float>(timer.GetDeltaTimeSeconds());

            uint32_t width = frameContext->GetSwapchain()->GetWidth();
            uint32_t height = frameContext->GetSwapchain()->GetHeight();
            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

            for (size_t i = 0; i < punctualLights.GetSize(); i++)
            {
                PunctualLight& light = punctualLights[i];
                PunctualLight& originalLight = originalsPunctualLights[i];

                light.Position.x += lightVelocity * deltaTime;
                float offset = light.Position.x - originalLight.Position.x;
                if (Math::Abs(offset) > lightThresholdPosition)
                {
                    lightVelocity = -lightVelocity;
                }
            }

            if (Input::KeyIsPressed(VirtualKey::C))
            {
                camera = initialCamera;
            }

            if (Input::KeyIsPressed(VirtualKey::F1))
            {
                application.ToogleUnlimitedFrameRate(Application::DefaultTargetFrameRate);
            }

            if (Input::KeyIsPressed(VirtualKey::Escape))
            {
                application.Stop();
            }

            Vector3f direction(0.0f, 0.0f, 0.0f);

            if (Input::KeyIsDown(VirtualKey::Z))
            {
                direction += camera.Front;
            }

            if (Input::KeyIsDown(VirtualKey::S))
            {
                direction -= camera.Front;
            }

            if (Input::KeyIsDown(VirtualKey::Q))
            {
                direction -= camera.Right;
            }

            if (Input::KeyIsDown(VirtualKey::D))
            {
                direction += camera.Right;
            }

            if (Input::KeyIsDown(VirtualKey::Space))
            {
                direction += camera.WorldUp;
            }

            if (Input::KeyIsDown(VirtualKey::LeftShift))
            {
                direction -= camera.WorldUp;
            }

            if (Vector3f::Length(direction) > 0.0f)
            {
                direction = Vector3f::Normalize(direction);
                camera.Position += direction * camera.MovementSpeed * deltaTime;
                camera.UpdateVectors();
            }

            camera.UpdateView();

            view.View = camera.View;
            view.Proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
            view.ViewProj = view.Proj * view.View;
            view.Eye = camera.Position;
        });

        PassContext passContext = {};
        passContext.FrameGraph = frameGraph;
        passContext.MaterialRegistry = materialRegistry;
        passContext.TextureRegistry = textureRegistry;
        passContext.PipelineManager = pipelineManager;

        // Render handle
        application.OnTick.Connect([&](Timer& /* timer */)
        {
            FrameResult result = frameContext->BeginFrame();
            WL_CHECK(result == FrameResult::Success);

            Frame& frame = frameContext->GetCurrentFrame();
            frameGraph->BeginFrame();

            uint32_t width = frameContext->GetSwapchain()->GetWidth();
            uint32_t height = frameContext->GetSwapchain()->GetHeight();

            Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
            Rect2D scissor(0.0f, 0.0f, width, height);

            // View Instance allocation
            RenderAllocation viewAllocation = frame.UniformAllocator.Allocate<RenderView>();
            frame.UniformAllocator.UpdateData(viewAllocation, view);

            // Light allocation
            RenderAllocation punctualLightBufferAllocation = frame.UniformAllocator.AllocateArray<PunctualLight>(punctualLights.GetSize());
            for (size_t i = 0; i < punctualLights.GetSize(); i++)
            {
                punctualLightBufferAllocation.Get<PunctualLight>()[i] = punctualLights[i];
            }

            RenderAllocation countersAllocation = frame.UniformAllocator.Allocate<uint32_t>();
            uint32_t* punctualLightCount = countersAllocation.Get<uint32_t>();
            *punctualLightCount = punctualLights.GetSize();

            // Sponza allocation
            RenderAllocation sponzaAllocation = frame.StorageAllocator.AllocateArray<RenderSubMeshData>(sponzaMesh.GetSubMeshCount());
            RenderSubMeshDataLayout layout = RenderSubMeshData::CreateLayout(frame.StorageAllocator.GetMinAligment());
            for (size_t i = 0; i < sponzaMesh.GetSubMeshCount(); i++)
            {
                layout.UpdateData(sponzaAllocation.Get<uint8_t>() + i * layout.Stride, sponzaMesh.GetSubMeshes()[i]);
            }

            FrameGraphBufferHandle indirect = frameGraph->ImportBuffer(indirectBuffer, indirectBuffer->GetSize(), 0);

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
            gBufferParams.DrawCount = drawIndexedCommands.GetSize();
            gBufferParams.MeshAllocation = &sponzaAllocation;
            gBufferParams.Mesh = &sponzaMesh;
            gBufferParams.RenderViewAllocation = &viewAllocation;

            FrameGraphPass& gBufferPass = GBufferPassCreate(passContext, gbufferPipelineProperties, gBufferParams);

            LightingPassParameters lightingParams = {};
            lightingParams.Color = color;
            lightingParams.Albedo = albedo;
            lightingParams.Normal = normal;
            lightingParams.Position = position;
            lightingParams.Indirect = indirect;
            lightingParams.DepthStencil = depthStencil;
            lightingParams.RenderViewAllocation = &viewAllocation;
            lightingParams.LightAllocation = &punctualLightBufferAllocation;
            lightingParams.Mesh = &uiMesh;
            lightingParams.MeshAllocation = &sponzaAllocation;
            lightingParams.CountersAllocation = &countersAllocation;

            FrameGraphPass& lightingPass = LightingPassCreate(passContext, lightingPipelineProperties, lightingParams);

            frameGraph->AddOutput(color);
            frameGraph->Compile();

            getOrCreatePipeline(gBufferPass,
                                gbufferPipelineProperties,
                                RHICullModeFlags::Back,
                                viewport,
                                scissor,
                                gBufferVertexShaderAsset,
                                gBufferFragmentShaderAsset);

            getOrCreatePipeline(lightingPass,
                                lightingPipelineProperties,
                                RHICullModeFlags::None,
                                viewport,
                                scissor,
                                lightingVertexShaderAsset,
                                lightingFragmentShaderAsset);

            frameGraph->Execute(frame.CommandBuffer);
            frameGraph->EndFrame();
            frameContext->EndFrame();
        });

        window->Show();

        application.Start();
        application.Run();

        frameGraph->Destroy();

        sponzaMesh.Destroy();
        uiMesh.Destroy();

        device->DestroyBuffer(indirectBuffer);

        pipelineManager->Destroy();

        materialRegistry->Destroy();
        textureRegistry->Destroy();

        frameContext->Destroy();
        device->Destroy();

        window->Close();

        application.Stop();

        return EXIT_SUCCESS;
    }

}// namespace Wl
