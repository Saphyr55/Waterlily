#include "LudoApplication.hpp"

#include "LudoAssets.hpp"
#include "LudoModule.hpp"
#include "LudoShaders.hpp"
#include "LudoState.hpp"
#include "LudoTypes.hpp"
#include "Passes/ForwardPass.hpp"
#include "Passes/UIPass.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/RHIModule.hpp"
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

    Camera InitCamera()
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
        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();

        LudoModule* ludoModule = moduleRegistry.GetModule<LudoModule>("Ludo");
        RHIModule* rhiModule = moduleRegistry.GetModule<RHIModule>("Waterlily.RHI");

        SharedPtr<Window> window = ludoModule->GetWindow();
        SharedPtr<RHIDevice> device = rhiModule->GetDevice();

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
        WL_CHECK(assetRegistryFileResult.HasValue());
        FileHandle& fileAssetRegistry = *assetRegistryFileResult.GetValue();

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
        frameContext->Init(frameContextInitInfo);

        SharedPtr<TextureRegistry> textureRegistry = MakeShared<TextureRegistry>(device, *assetManager, LudoTexturesSRGBinding);
        SharedPtr<MaterialRegistry> materialRegistry = MakeShared<MaterialRegistry>(device, LudoMaterialsSRGBinding);
        SharedPtr<PipelineManager> pipelineManager = MakeShared<PipelineManager>(device, frameContext->GetSRGLayoutCache(), assetsFileSystem);

        Model* sponzaModelAsset = assetManager->GetAsset<Model>(LudoAssetModelSponza);
        WL_CHECK(sponzaModelAsset);

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
            WL_LOG_DEBUG(
                    "[Ludo]",
                    Wl::Format("Flushed global upload scheduler, total uploaded: %.2lfMB", megaByte));
        }, device->GetGraphicsQueue());

        uploadScheduler.Shutdown();

        textureRegistry->CompileSRG();
        materialRegistry->CompileShaderResource();

        frameContext->InitializeSRGPools();

        SharedPtr<FrameGraph> frameGraph = MakeShared<FrameGraph>(device, frameContext);

        LudoState::Type state = LudoState::Type::Running;

        window->GetEventHandler().OnClose.Connect([&]()
        {
            WL_LOG_INFO("[Ludo]", "Window closed.");
            application.Stop();
        });

        window->GetEventHandler().OnResized.Connect([&](uint32_t width, uint32_t height)
        {
            WL_LOG_INFO("[Ludo]", Wl::Format("Window resized to %dx%d", width, height));
            frameContext->Resize(width, height);
            frameGraph->Dispose();
        });

        window->GetEventHandler().OnMinimized.Connect([&]()
        {
            state = LudoState::Type::Paused;
        });

        window->GetEventHandler().OnShown.Connect([&]()
        {
            state = LudoState::Type::Running;
        });

        GraphicsPipelineProperties forwardPipelineProperties = {};
        GraphicsPipelineProperties uiPipelineProperties = {};

        Input::OnKeyRelease.Connect([&](VirtualKey key)
        {
            switch (key)
            {
                case VirtualKey::G: {
                    HashMap<StringID, GraphicsPipelineProperties*> propsMap = {
                            {LudoForwardPassName, &forwardPipelineProperties},
                            {LudoUIPassName,      &uiPipelineProperties     }
                    };
                    Wl::ReloadShaders(device, pipelineManager, propsMap);
                    break;
                };
                case VirtualKey::P: {
                    state = LudoState::TooglePause(state);
                    break;
                };
                case VirtualKey::F: {
                    camera.LogDebug();
                    break;
                }
                default:
                    break;
            }
        });

        // Update handle
        application.OnTick.Connect([&](double deltaTime)
        {
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
        application.OnTick.Connect([&](double /* deltaTime */)
        {
            WL_RETURN_WHEN(LudoState::IsPaused(state));

            FrameResult result = frameContext->BeginFrame();
            WL_CHECK(result == FrameResult::Success);

            frameGraph->BeginFrame();

            Frame& frame = frameContext->GetCurrentFrame();

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
            RenderAllocation lightAllocation = frame.UniformAllocator.Allocate<PunctualLight>();
            PunctualLight* light = lightAllocation.Get<PunctualLight>();
            light->Position = Vector3f(0.0f, 2.0f, 0.5f);
            light->Color = Vector3f(1.0f, 0.3f, 0.0f);
            frame.UniformAllocator.UpdateData(lightAllocation, *light);

            // Sponza allocation
            RenderAllocation sponzaAllocation = frame.StorageAllocator.AllocateArray<RenderSubMeshData>(sponzaMesh.GetSubMeshCount());
            RenderSubMeshDataLayout layout = RenderSubMeshData::CreateLayout(frame.StorageAllocator.GetMinAligment());
            uint8_t* subMeshes = sponzaAllocation.Get<uint8_t>();

            for (size_t i = 0; i < sponzaMesh.GetSubMeshCount(); i++)
            {
                const RenderSubMesh& subMesh = sponzaMesh.GetSubMeshes()[i];
                const RenderSubMeshData data = RenderSubMeshData::Create(subMesh);
                uint8_t* dest = subMeshes + i * layout.Stride;
                // TODO: Flexible way to handle the layout
                Memory::Copy(dest + layout.ModelOffset, &data.Model, sizeof(Matrix4f));
                Memory::Copy(dest + layout.MaterialOffset, &data.Material, sizeof(MaterialHandle));
            }

            FrameGraphBufferHandle indirect = frameGraph->ImportBuffer(indirectBuffer, indirectBuffer->GetSize(), 0);

            FrameGraphTextureInfo colorTextureInfo = {};
            colorTextureInfo.Name = "Color";
            colorTextureInfo.Format = frameContext->GetSwapchain()->GetFormat();
            colorTextureInfo.SizeClass = SizeClass::Swapchain;
            FrameGraphTextureHandle color = frameGraph->CreateTexture(colorTextureInfo);

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

            ForwardPassParameters forwardParams = {};
            forwardParams.Color = color;
            forwardParams.DepthStencil = depthStencil;
            forwardParams.Indirect = indirect;
            forwardParams.DrawCount = drawIndexedCommands.GetSize();
            forwardParams.MeshAllocation = &sponzaAllocation;
            forwardParams.Mesh = &sponzaMesh;
            forwardParams.RenderViewAllocation = &viewAllocation;
            forwardParams.LightAllocation = &lightAllocation;

            FrameGraphPass& forwardPass = ForwardPassCreate(passContext, forwardPipelineProperties, forwardParams);

            UIPassParameters uiParams = {};
            uiParams.Color = color;
            uiParams.Mesh = &uiMesh;

            FrameGraphPass& uiPass = UIPassCreate(passContext, uiPipelineProperties, uiParams);

            frameGraph->AddOutput(color);
            frameGraph->Compile();

            Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
            Rect2D scissor(0.0f, 0.0f, width, height);

            // Setup forward pipeline
            forwardPipelineProperties.RenderPass = frameGraph->GetRenderPass(LudoForwardPassName);
            forwardPipelineProperties.CullMode = RHICullModeFlags::Back;
            forwardPipelineProperties.VertexShaderPath = LudoAssetSPVVertexShaderForward;
            forwardPipelineProperties.FragmentShaderPath = LudoAssetSPVFragmentShaderForward;
            forwardPipelineProperties.Scissor = scissor;
            forwardPipelineProperties.Viewport = viewport;
            forwardPipelineProperties.SRGLayouts[LudoTextureGRGIndex] = textureRegistry->GetSRGLayout();
            forwardPipelineProperties.SRGLayouts[LudoMaterialsSRGIndex] = materialRegistry->GetSRGLayout();

            // Setup ui pipeline
            uiPipelineProperties.RenderPass = frameGraph->GetRenderPass(LudoUIPassName);
            uiPipelineProperties.CullMode = RHICullModeFlags::None;
            uiPipelineProperties.VertexShaderPath = LudoAssetSPVVertexShaderUI;
            uiPipelineProperties.FragmentShaderPath = LudoAssetSPVFragmentShaderUI;
            uiPipelineProperties.Scissor = scissor;
            uiPipelineProperties.Viewport = viewport;
            uiPipelineProperties.SRGLayouts[LudoTextureGRGIndex] = textureRegistry->GetSRGLayout();

            // Creating pipelines.
            pipelineManager->GetOrCreate(forwardPass.GetName(), forwardPipelineProperties);
            pipelineManager->GetOrCreate(uiPass.GetName(), uiPipelineProperties);

            frameGraph->Execute(frame.CommandBuffer);
            frameGraph->EndFrame();
            frameContext->EndFrame();
        });

        // Input Handle
        application.OnTick.Connect([&](double /* deltaTime */)
        {
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

            Display::GetDefault().HandleEvents();
        });

        window->Show();

        application.Start();
        application.Run();

        frameGraph->Dispose();

        sponzaMesh.Destroy();
        uiMesh.Destroy();

        device->DestroyBuffer(indirectBuffer);

        pipelineManager->Dispose();

        textureRegistry->Dispose();
        frameContext->Shutdown();

        window->Close();

        application.Stop();

        return EXIT_SUCCESS;
    }

}// namespace Wl
