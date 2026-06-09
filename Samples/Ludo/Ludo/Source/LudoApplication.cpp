#include "LudoApplication.hpp"

#include "LudoAssets.hpp"
#include "LudoShaders.hpp"
#include "LudoState.hpp"
#include "LudoTypes.hpp"
#include "Passes/GBufferPass.hpp"
#include "Passes/LightingPass.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
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
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
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
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PunctualLight.hpp"

namespace Wl
{
    static FixedArray<PunctualLight, 5> MakeLights()
    {
        FixedArray<PunctualLight, 5> lights;

        lights[0] = {
                {3.49f, 3.49f, -2.34f},
                {0.1f, 0.6f, 0.9f},
        };

        lights[1] = {
                {-5.76f, 1.22f, -0.82f},
                {1.0f, 0.6f, 0.1f},
        };

        lights[2] = {
                {-0.46f, 3.63f, 1.88f},
                {0.4f,  1.0f, 0.2f},
        };

        lights[3] = {
                {-2.07f, 1.94f, 0.04f},
                {0.0f, 0.5f, 1.0f},
        };

        lights[4] = {
                {-6.5f, 0.75f, 0.5f},
                {0.4f, 0.8f, 0.8f},
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
        // Create a window. WindowHandle behave like a reference.
        WindowProperties windowProperties("Demo Window", 1080, 720, 100, 100);
        SharedPtr<Window> window = Window::Create(windowProperties);

        // Need to know the window for creating the surface. Vulkan API is already the default option.
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

        WL_CHECK_MSG(Wl::CompileShaders(), "Failed to compile shaders.");

        FileSystem& assetsFileSystem = FileSystem::GetPlatform();

        FileResult assetRegistryFileResult = assetsFileSystem.OpenRead(LudoAssetRegistry.GetText());
        WL_CHECK_MSG(assetRegistryFileResult.HasValue(), "Impossible to read \"%s\"", LudoAssetRegistry.GetText().GetData());
        File& fileAssetRegistry = *assetRegistryFileResult.GetValue();

        SharedPtr<AssetRegistry> assetRegistry = AssetRegistry::LoadFromFile(fileAssetRegistry);
        WL_CHECK(assetRegistry);
        fileAssetRegistry.Close();

        SharedPtr<IAssetLoader> assetLoader = MakeShared<LCAAssetLoader>(assetsFileSystem);
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

        Model* sponzaModelAsset = assetManager->GetAsset<Model>(LudoAssetModelSponza);
        WL_CHECK_MSG(sponzaModelAsset, "Impossible to load \"%s\" asset.", LudoAssetModelSponza.GetText().data());

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

        SharedPtr<FrameGraph> frameGraph = MakeShared<FrameGraph>(device, frameContext);

        LudoState::Type state = LudoState::Type::Running;

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

        window->GetEventHandler().OnMinimized.Connect([&]()
        {
            state = LudoState::Type::Paused;
        });

        window->GetEventHandler().OnShown.Connect([&]()
        {
            state = LudoState::Type::Running;
        });

        GraphicsPipelineProperties gbufferPipelineProperties = {};
        GraphicsPipelineProperties lightingPipelineProperties = {};

        Input::OnKeyRelease.Connect([&](VirtualKey key)
        {
            switch (key)
            {
                case VirtualKey::G:
                {
                    HashMap<StringID, GraphicsPipelineProperties*> propsMap = {
                            {GBufferPassName,  &gbufferPipelineProperties },
                            {LightingPassName, &lightingPipelineProperties}
                    };
                    Wl::ReloadShaders(device, pipelineManager, propsMap);
                    break;
                }
                case VirtualKey::P:
                {
                    state = LudoState::TooglePause(state);
                    break;
                }
                case VirtualKey::F:
                {
                    camera.LogDebug();
                    break;
                }
                default:
                    break;
            }
        });

        float lightVelocity = 1.0f;
        float lightThresholdPosition = 5.0f; 

        // Update handle
        application.OnTick.Connect([&](Timer& timer)
        {
            float deltaTime = static_cast<float>(timer.GetDeltaTimeSeconds());

            if (LudoState::IsPaused(state))
            {
                return;
            }

            for (size_t i = 0; i < punctualLights.GetSize(); i++)
            {
                PunctualLight& light = punctualLights[i];
                PunctualLight& originalLight = originalsPunctualLights[i];

                light.Position.x += lightVelocity * deltaTime;

                if (Math::Abs(light.Position.x) > originalLight.Position.x + lightThresholdPosition)
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
        });

        // Render handle
        application.OnTick.Connect([&](Timer& /* timer */)
        {
            if (LudoState::IsPaused(state))
            {
                return;
            }

            FrameResult result = frameContext->BeginFrame();
            WL_CHECK(result == FrameResult::Success);

            Frame& frame = frameContext->GetCurrentFrame();
            frameGraph->BeginFrame();

            uint32_t width = frameContext->GetSwapchain()->GetWidth();
            uint32_t height = frameContext->GetSwapchain()->GetHeight();
            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

            // View Instance allocation
            RenderAllocation viewAllocation = frame.UniformAllocator.Allocate<RenderView>();
            RenderView* view = viewAllocation.Get<RenderView>();
            view->View = camera.View;
            view->Proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
            view->ViewProj = view->Proj * view->View;
            view->Eye = camera.Position;
            frame.UniformAllocator.UpdateData(viewAllocation, *view);

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

            PassContext passContext = {};
            passContext.FrameGraph = frameGraph;
            passContext.MaterialRegistry = materialRegistry;
            passContext.TextureRegistry = textureRegistry;
            passContext.PipelineManager = pipelineManager;

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

            Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
            Rect2D scissor(0.0f, 0.0f, width, height);

            gbufferPipelineProperties.RenderPass = frameGraph->GetRenderPass(GBufferPassName);
            gbufferPipelineProperties.CullMode = RHICullModeFlags::Back;
            gbufferPipelineProperties.VertexShaderPath = AssetSPVVertexShaderGBuffer;
            gbufferPipelineProperties.FragmentShaderPath = AssetSPVFragmentShaderGBuffer;
            gbufferPipelineProperties.Scissor = scissor;
            gbufferPipelineProperties.Viewport = viewport;
            gbufferPipelineProperties.SRGLayouts[LudoTextureGRGIndex] = textureRegistry->GetSRGLayout();
            gbufferPipelineProperties.SRGLayouts[LudoMaterialsSRGIndex] = materialRegistry->GetSRGLayout();

            lightingPipelineProperties.RenderPass = frameGraph->GetRenderPass(LightingPassName);
            lightingPipelineProperties.CullMode = RHICullModeFlags::None;
            lightingPipelineProperties.VertexShaderPath = AssetSPVVertexShaderLighting;
            lightingPipelineProperties.FragmentShaderPath = AssetSPVFragmentShaderLigthing;
            lightingPipelineProperties.Scissor = scissor;
            lightingPipelineProperties.Viewport = viewport;
            lightingPipelineProperties.SRGLayouts[LudoTextureGRGIndex] = textureRegistry->GetSRGLayout();
            lightingPipelineProperties.SRGLayouts[LudoMaterialsSRGIndex] = materialRegistry->GetSRGLayout();

            pipelineManager->GetOrCreate(gBufferPass.GetName(), gbufferPipelineProperties);
            pipelineManager->GetOrCreate(lightingPass.GetName(), lightingPipelineProperties);

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
