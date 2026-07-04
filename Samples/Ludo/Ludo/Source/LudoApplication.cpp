#include "LudoApplication.hpp"

#include "Passes/GBufferPass.hpp"
#include "Passes/LightingPass.hpp"
#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/Platform/Window.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
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
#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Renderer/View.hpp"
#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Scene/PunctualLight.hpp"

namespace Wl
{

    static const StringID AssetRegistryURI = WL_SID("Assets/Registry.wlar");
    static const StringID SponzaModelAssetURI = WL_SID("Assets/Models/Sponza.wlca");

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

    LudoApplicationDelegate::LudoApplicationDelegate()
        : m_sponzaModelAsset(nullptr)
        , m_indirectBuffer(nullptr)
        , m_indirectBufferCount(0)
    {
    }

    void LudoApplicationDelegate::OnStartup()
    {
        m_window = Window::Create(WindowProperties("Demo Window", 1080, 720, 100, 100));

        m_device = RHIDeviceFactory::Create(RHIGraphicsAPI::Vulkan);
        m_device->Init(m_window->GetNativeWindow());

        m_camera = InitCamera();

        Input::OnMouseMove.Connect([this](const MouseMove& mouseMove)
        {
            if (Input::ButtonIsDown(Button::Left))
            {
                m_camera.LookAround(mouseMove.PosRelX, -mouseMove.PosRelY);
            }
        });

        FileSystem& assetsFileSystem = FileSystem::GetPlatform();

        FileResult assetRegistryFileResult = assetsFileSystem.OpenRead(AssetRegistryURI.GetText());
        WL_CHECK_MSG(assetRegistryFileResult.HasValue(), "Impossible to read \"%s\"", AssetRegistryURI.GetText().GetData());
        SharedPtr<File> fileAssetRegistry = assetRegistryFileResult.GetValue();

        m_assetRegistry = AssetRegistry::LoadFromFile(*fileAssetRegistry);
        WL_CHECK(m_assetRegistry);
        fileAssetRegistry->Close();

        SharedPtr<AssetLoader> assetLoader = MakeShared<ConditionnedAssetLoader>(assetsFileSystem);
        m_assetManager = MakeShared<AssetManager>(m_assetRegistry, assetLoader);

        FrameContextInitInfo frameContextInitInfo = {};
        frameContextInitInfo.StagingBufferSize = 16 * WL_MB;
        frameContextInitInfo.StorageBufferSize = 16 * WL_MB;
        frameContextInitInfo.UniformBufferSize = 16 * WL_MB;
        frameContextInitInfo.FrameWidth = m_window->GetProperties().Width;
        frameContextInitInfo.FrameHeight = m_window->GetProperties().Height;

        m_frameContext = MakeShared<FrameContext>(m_device);
        m_frameContext->Init(frameContextInitInfo);

        m_textureRegistry = MakeShared<TextureRegistry>(m_device, *m_assetManager, LudoTexturesSRGBinding);
        m_materialRegistry = MakeShared<MaterialRegistry>(m_device, LudoMaterialsSRGBinding);
        m_pipelineManager = MakeShared<PipelineManager>(m_device, m_frameContext->GetSRGLayoutCache(), assetsFileSystem);

        m_shaderBundle = MakeShared<ShaderBundle>();
        m_shaderBundle->CreateAssets(m_assetRegistry);

        // This should be not done in application mode, but only in dev mode.
        bool isCompilingShaderSuccessed = m_shaderBundle->LoadAssets(m_assetManager);
        WL_CHECK_MSG(isCompilingShaderSuccessed, "Failed to compile shaders.");

        m_sponzaModelAsset = m_assetManager->GetAsset<Model>(SponzaModelAssetURI);
        WL_CHECK_MSG(m_sponzaModelAsset, "Impossible to load \"%s\" asset.", SponzaModelAssetURI.GetText().GetData());

        Array<StaticMesh*> modelStaticMeshesAsset(m_sponzaModelAsset->Meshes.GetSize());
        for (const AssetHandle& meshAssetHandle: m_sponzaModelAsset->Meshes)
        {
            modelStaticMeshesAsset.Append(m_assetManager->GetAsset<StaticMesh>(meshAssetHandle));
        }

        UploadScheduler uploadScheduler;
        UploadSchedulerInitInfo uploadSchedulerInit = {};
        uploadSchedulerInit.Device = m_device;
        uploadSchedulerInit.StagingSize = 16 * WL_MB;
        uploadSchedulerInit.MinAlignment = m_device->GetDeviceProperties().NonCoherentAtomSize;
        uploadScheduler.Init(uploadSchedulerInit);

        m_sponzaMesh = MakeShared<RenderMesh>(m_device);
        // TODO: We should create one big mesh to send.
        m_sponzaMesh->Instanciate(uploadScheduler, m_assetManager, m_textureRegistry, m_materialRegistry, modelStaticMeshesAsset[0]);

        for (RenderSubMesh& subMesh: m_sponzaMesh->GetSubMeshes())
        {
            subMesh.Model = Matrix4f::Scale(subMesh.Model, Vector3f(0.005f));
        }

        Array<RHIDrawIndexedCommand> drawIndexedCommands = m_sponzaMesh->CreateDrawIndexedCommands();
        m_indirectBuffer = m_device->CreateIndirectBuffer(drawIndexedCommands);
        m_indirectBufferCount = drawIndexedCommands.GetSize();

        uploadScheduler.Upload(ArrayView(drawIndexedCommands), m_indirectBuffer);
        m_textureRegistry->Upload();
        m_materialRegistry->Upload(m_device->GetGraphicsQueue());

        m_device->ImediateSubmit([uploadScheduler](RHICommandBuffer* commandBuffere) mutable
        {
            double byte = static_cast<double>(uploadScheduler.GetTotalPendingBytes());
            double megaByte = byte / static_cast<double>(WL_MB);
            uploadScheduler.Flush(commandBuffere);
            WL_LOG_DEBUG("Ludo", "Flushed global upload scheduler, total uploaded: %.2lfMB", megaByte);
        }, m_device->GetGraphicsQueue());

        uploadScheduler.Shutdown();

        m_textureRegistry->CompileSRG();
        m_materialRegistry->CompileShaderResource();

        m_frameContext->InitializeSRGPools();

        m_originalLights = MakeLights();
        m_lights = MakeLights();

        m_frameGraph = MakeShared<FrameGraph>(m_frameContext);

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
            switch (key)
            {
                case VirtualKey::F2:
                {
                    m_device->WaitIdle();

                    m_shaderBundle->ReloadAssets(m_assetManager);

                    HashMap<StringID, GraphicsPipelineState*> propsMap = {
                            {GBufferPassName,  &m_shaderBundle->GBufferPipelineProperties },
                            {LightingPassName, &m_shaderBundle->LightingPipelineProperties}
                    };
                    for (auto [name, props]: propsMap)
                    {
                        m_pipelineManager->Recreate(name, *props);
                    }
                    break;
                }
                case VirtualKey::F3:
                {
                    m_camera.LogDebug();
                    break;
                }
                case Wl::VirtualKey::F4:
                {
                    m_camera = InitCamera();
                    break;
                }
                case VirtualKey::Escape:
                {
                    Engine::GetInstance().GetApplication()->RequestStop();
                    break;
                }
                default:
                    break;
            }
        });

        m_window->Show();
    }

    void LudoApplicationDelegate::OnUpdate(double deltaTime)
    {
        uint32_t width = m_frameContext->GetSwapchain()->GetWidth();
        uint32_t height = m_frameContext->GetSwapchain()->GetHeight();

        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        
        static float elapsedTime = 0.0f;
        
        float lightVelocity = 3.0f; 
        float lightAmplitude = 0.5f;

        elapsedTime += deltaTime;

        for (size_t i = 0; i < m_lights.GetSize(); i++)
        {
            PunctualLight& light = m_lights[i];
            PunctualLight& originalLight = m_originalLights[i];

            float offset = Math::Sin(elapsedTime * lightVelocity) * lightAmplitude;
            light.Position = originalLight.Position + Wl::Vector3(offset, 0.0f, 0.0f);
        }

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
            m_camera.Position += direction * m_camera.MovementSpeed * deltaTime;
            m_camera.UpdateVectors();
        }

        m_camera.UpdateView();

        m_view.View = m_camera.View;
        m_view.Proj = Matrix4f::Perspective(Math::Radians(75.0f), aspectRatio, 0.1f, 1000.0f);
        m_view.ViewProj = m_view.Proj * m_view.View;
        m_view.Eye = m_camera.Position;
    }

    void LudoApplicationDelegate::OnRender()
    {
        PassContext passContext = {};
        passContext.FrameGraph = m_frameGraph;
        passContext.MaterialRegistry = m_materialRegistry;
        passContext.TextureRegistry = m_textureRegistry;
        passContext.PipelineManager = m_pipelineManager;

        FrameResult result = m_frameContext->BeginFrame();
        WL_CHECK(result == FrameResult::Success);

        m_frameGraph->BeginFrame();

        Frame& frame = m_frameContext->GetCurrentFrame();

        uint32_t width = m_frameContext->GetSwapchain()->GetWidth();
        uint32_t height = m_frameContext->GetSwapchain()->GetHeight();

        Viewport viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        Rect2D scissor(0.0f, 0.0f, width, height);

        // View Instance allocation
        RenderAllocation viewAllocation = frame.UniformAllocator.Allocate<ViewData>();
        frame.UniformAllocator.UpdateData(viewAllocation, m_view);

        // Light allocation
        RenderAllocation punctualLightBufferAllocation = frame.UniformAllocator.AllocateArray<PunctualLight>(m_lights.GetSize());
        for (size_t i = 0; i < m_lights.GetSize(); i++)
        {
            punctualLightBufferAllocation.Get<PunctualLight>()[i] = m_lights[i];
        }

        RenderAllocation countersAllocation = frame.UniformAllocator.Allocate<uint32_t>();
        uint32_t* punctualLightCount = countersAllocation.Get<uint32_t>();
        *punctualLightCount = m_lights.GetSize();

        // Sponza allocation
        RenderAllocation sponzaAllocation = frame.StorageAllocator.AllocateArray<RenderSubMeshData>(m_sponzaMesh->GetSubMeshCount());
        RenderSubMeshDataLayout layout = RenderSubMeshData::CreateLayout(frame.StorageAllocator.GetMinAligment());
        for (size_t i = 0; i < m_sponzaMesh->GetSubMeshCount(); i++)
        {
            layout.UpdateData(sponzaAllocation.Get<uint8_t>() + i * layout.Stride, m_sponzaMesh->GetSubMeshes()[i]);
        }

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
        gBufferParams.DrawCount = m_indirectBufferCount;
        gBufferParams.MeshAllocation = &sponzaAllocation;
        gBufferParams.Mesh = m_sponzaMesh;
        gBufferParams.RenderViewAllocation = &viewAllocation;

        FrameGraphPass& gBufferPass = GBufferPassCreate(passContext, m_shaderBundle->GBufferPipelineProperties, gBufferParams);

        LightingPassParameters lightingParams = {};
        lightingParams.Color = color;
        lightingParams.Albedo = albedo;
        lightingParams.Normal = normal;
        lightingParams.Position = position;
        lightingParams.Indirect = indirect;
        lightingParams.DepthStencil = depthStencil;
        lightingParams.RenderViewAllocation = &viewAllocation;
        lightingParams.LightAllocation = &punctualLightBufferAllocation;
        lightingParams.MeshAllocation = &sponzaAllocation;
        lightingParams.CountersAllocation = &countersAllocation;

        FrameGraphPass& lightingPass = LightingPassCreate(passContext, m_shaderBundle->LightingPipelineProperties, lightingParams);

        m_frameGraph->AddOutput(color);
        m_frameGraph->Compile();

        GetOrCreatePipeline(gBufferPass,
                            m_shaderBundle->GBufferPipelineProperties,
                            RHICullModeFlags::Back,
                            viewport,
                            scissor);

        GetOrCreatePipeline(lightingPass,
                            m_shaderBundle->LightingPipelineProperties,
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
        state.SRGLayouts[LudoTextureGRGIndex] = m_textureRegistry->GetSRGLayout();
        state.SRGLayouts[LudoMaterialsSRGIndex] = m_materialRegistry->GetSRGLayout();
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
