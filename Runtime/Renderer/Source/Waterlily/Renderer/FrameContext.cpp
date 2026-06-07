#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/RHIModule.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/RHI/ShaderResourcePool.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    void FrameContext::Init(const FrameContextInitInfo& info)
    {
        WL_CHECK_MSG(info.GraphicsCommandBufferCount > 0, "Must have at least 1 graphics command buffer.");

        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();
        Display& display = Display::GetDefault();

        RHIModule* rhiModule = moduleRegistry.GetModule<RHIModule>("Waterlily.RHI");

        m_device = rhiModule->GetDevice();

        m_swapchain = m_device->CreateSwapchain(info.FrameWidth, info.FrameWidth, m_maxFrameInFlight);

        m_frameInFlightFences.Resize(m_swapchain->GetTextureViews().GetSize(), nullptr);

        constexpr uint32_t renderFrameFramebufferIndex = 0;

        m_srgLayoutCache = MakeShared<RHIShaderResourceGroupLayoutCache>(m_device);

        const RHIDeviceProperties& properties = m_device->GetDeviceProperties();

        for (Frame& frame: m_frames)
        {
            frame.CommandAllocator = m_device->CreateCommandAllocatorr({
                    .CommandQueue = m_device->GetGraphicsQueue(),
                    .Count = info.GraphicsCommandBufferCount,
            });

            frame.CommandBuffer = frame.CommandAllocator->OpenCommandBuffer(renderFrameFramebufferIndex);

            frame.FrameAvailableSemaphore = m_device->CreateSemaphore();
            frame.RenderFinishedSemaphore = m_device->CreateSemaphore();
            frame.InFlightFence = m_device->CreateFence();

            RHIBuffer* uniformBuffer = m_device->CreateBuffer(RHIBufferDescription {
                    .Size = info.UniformBufferSize,
                    .Usage = RHIBufferUsageFlags::Uniform,
                    .MemoryUsage = RHIMemoryUsage::Host,
                    .SharingMode = RHISharingMode::Private,
            });

            frame.UniformAllocator.Initialize(uniformBuffer, properties.MinUniformBufferOffsetAlignment);

            RHIBuffer* storageBuffer = m_device->CreateBuffer(RHIBufferDescription {
                    .Size = info.StorageBufferSize,
                    .Usage = RHIBufferUsageFlags::Storage,
                    .MemoryUsage = RHIMemoryUsage::Host,
                    .SharingMode = RHISharingMode::Private,
            });

            frame.StorageAllocator.Initialize(storageBuffer, properties.MinStorageBufferOffsetAlignment);

            frame.Uploader.Init({
                    .Device = m_device,
                    .StagingSize = info.StagingBufferSize,
                    .MinAlignment = properties.NonCoherentAtomSize,
            });
        }
    }

    void FrameContext::InitializeSRGPools()
    {
        for (Frame& frame: m_frames)
        {
            Array<RHIShaderResourceBinding> totalBindings;
            // FIXME: This is absurd because we have to loop through all frames and all their SRG layouts in cache (so
            // probably not entirely filled) to get the total bindings to create the pool. By using the FrameGraph we can
            // easily get the total bindings for all SRG layouts used in the frame and create the pool without this absurd
            // loop.
            for (const RHIShaderResourceGroupLayout* srgLayout: m_srgLayoutCache->GetResources())
            {
                totalBindings.AppendRange(srgLayout->GetBindings());
            }

            frame.SRGPool = m_device->CreateSRGPool(100, totalBindings);
        }
    }

    void FrameContext::Resize(uint32_t width, uint32_t height)
    {

        m_device->WaitIdle();
        m_device->RecreateSwapchain(m_swapchain, width, height);
    }

    void FrameContext::Shutdown()
    {
        m_device->WaitIdle();

        for (Frame& frame: m_frames)
        {
            m_device->DestroyCommandAllocator(frame.CommandAllocator);
            m_device->DestroySemaphore(frame.FrameAvailableSemaphore);
            m_device->DestroySemaphore(frame.RenderFinishedSemaphore);
            m_device->DestroyFence(frame.InFlightFence);

            m_device->DestroyBuffer(static_cast<RHIBuffer*>(frame.UniformAllocator.GetBuffer()));
            m_device->DestroyBuffer(static_cast<RHIBuffer*>(frame.StorageAllocator.GetBuffer()));
            frame.Uploader.Shutdown();

            m_device->DestroySRGPool(frame.SRGPool);
        }

        m_srgLayoutCache->Dispose();
        m_device->DestroySwapchain(m_swapchain);
    }

    FrameResult FrameContext::BeginFrame()
    {
        Frame& frame = GetCurrentFrame();

        frame.UniformAllocator.Reset();
        frame.StorageAllocator.Reset();
        frame.Uploader.Reset();
        frame.SRGPool->Reset();

        RHISwapchainAcquireResult result = m_swapchain->AcquireNextFrame(frame.FrameAvailableSemaphore);

        if (result.IsOutOfDate || result.IsNotReady)
        {
            m_device->RecreateSwapchain(m_swapchain, m_swapchain->GetWidth(), m_swapchain->GetHeight());
        }

        frame.CommandAllocator->ResetCommandBuffer(frame.CommandBuffer);
        frame.CommandBuffer->Begin();

        return FrameResult::Success;
    }

    void FrameContext::EndFrame()
    {
        Frame& frame = GetCurrentFrame();

        frame.CommandBuffer->End();

        RHISwapchainAcquireResult& result = m_swapchain->GetCurrentAcquireResult();
        m_frameInFlightFences[result.ImageIndex] = &frame.InFlightFence;

        m_device->ResetFence(frame.InFlightFence);

        m_device->GetGraphicsQueue()->Submit({frame.CommandBuffer},
                                             {frame.FrameAvailableSemaphore},
                                             {frame.RenderFinishedSemaphore},
                                             frame.InFlightFence);

        m_device->GetPresentQueue()->Present(m_swapchain, frame.RenderFinishedSemaphore);
        m_device->WaitFence(frame.InFlightFence);

        NextFrame();
    }

}// namespace Wl