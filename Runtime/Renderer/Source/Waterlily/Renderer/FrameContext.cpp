#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/ShaderResourcePool.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    void FrameContext::Init(const FrameContextInitInfo& info)
    {
        WL_CHECK_MSG(info.GraphicsCommandBufferCount > 0, "Must have at least 1 graphics command buffer.");

        Display& display = Display::GetDefault();

        m_swapchain = m_device->CreateSwapchain(info.FrameWidth, info.FrameWidth, m_maxFrameInFlight);

        m_frameInFlightFences.Resize(m_swapchain->GetTextureViews().GetSize(), nullptr);

        constexpr uint32_t renderFrameFramebufferIndex = 0;

        const RHIDeviceProperties& properties = m_device->GetDeviceProperties();

        for (Frame& frame: m_frames)
        {
            frame.CommandAllocator = m_device->CreateCommandAllocatorr({
                    .CommandQueue = m_device->GetGraphicsQueue(),
                    .Count = info.GraphicsCommandBufferCount,
            });

            frame.CommandBuffer = frame.CommandAllocator->OpenCommandBuffer(renderFrameFramebufferIndex);

            frame.FrameAvailableSemaphore = m_device->CreateSemaphore();

            for (size_t i = 0; i < m_swapchain->GetTextureViews().GetSize(); i++)
            {
                frame.RenderFinishedSemaphore.Append(m_device->CreateSemaphore());
            }
            
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

            frame.Uploader.Init(UploadSchedulerInitInfo {
                    .Device = m_device,
                    .StagingSize = info.StagingBufferSize,
                    .MinAlignment = properties.NonCoherentAtomSize,
            });
        }
    }

    void FrameContext::Resize(uint32_t width, uint32_t height)
    {
        m_device->WaitIdle();
        m_device->RecreateSwapchain(m_swapchain, width, height);
    }

    uint32_t FrameContext::GetWidth() const
    {
        return m_swapchain->GetWidth();
    }

    uint32_t FrameContext::GetHeight() const
    {
        return m_swapchain->GetHeight();
    }

    float FrameContext::GetAspectRatio() const
    {
        return static_cast<float>(m_swapchain->GetWidth()) / static_cast<float>(m_swapchain->GetHeight());
    }

    void FrameContext::Destroy()
    {
        m_device->WaitIdle();

        for (Frame& frame: m_frames)
        {
            m_device->DestroyCommandAllocator(frame.CommandAllocator);
            m_device->DestroySemaphore(frame.FrameAvailableSemaphore);
            for (size_t i = 0; i < m_swapchain->GetTextureViews().GetSize(); i++)
            {
                m_device->DestroySemaphore(frame.RenderFinishedSemaphore[i]);
            }
            m_device->DestroyFence(frame.InFlightFence);

            m_device->DestroyBuffer(static_cast<RHIBuffer*>(frame.UniformAllocator.GetBuffer()));
            m_device->DestroyBuffer(static_cast<RHIBuffer*>(frame.StorageAllocator.GetBuffer()));

            frame.Uploader.Shutdown();

            m_device->DestroySRGPool(frame.SRGPool);
        }

        m_device->DestroySwapchain(m_swapchain);
    }

    FrameResult FrameContext::BeginFrame()
    {
        Frame& frame = GetCurrentFrame();

        RHISwapchainAcquireResult result = m_swapchain->AcquireNextFrame(frame.FrameAvailableSemaphore);

        if (result.IsOutOfDate || result.IsNotReady)
        {
            m_device->RecreateSwapchain(m_swapchain, m_swapchain->GetWidth(), m_swapchain->GetHeight());
        }

        if (RHIFence** fence = m_frameInFlightFences[result.ImageIndex])
        {
            m_device->WaitFence(*fence);
        }

        m_frameInFlightFences[result.ImageIndex] = &frame.InFlightFence;

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
                                             {frame.RenderFinishedSemaphore[result.ImageIndex]},
                                             frame.InFlightFence);

        m_device->GetPresentQueue()->Present(m_swapchain, frame.RenderFinishedSemaphore[result.ImageIndex]);
        m_device->WaitFence(frame.InFlightFence);

        frame.UniformAllocator.Reset();
        frame.StorageAllocator.Reset();
        frame.Uploader.Reset();
        frame.SRGPool->Reset();


        NextFrame();
    }

    SharedPtr<RHIDevice> FrameContext::GetDevice() const
    {
        return m_device;
    }

    uint64_t FrameContext::GetFrameIndex() const
    {
        return m_frameIndex;
    }

    uint64_t FrameContext::GetMaxFrameInFlight() const
    {
        return m_maxFrameInFlight;
    }

    uint64_t FrameContext::GetFrameCount() const
    {
        return m_frameCount;
    }

    Frame& FrameContext::GetCurrentFrame()
    {
        return m_frames[m_frameIndex];
    }

    void FrameContext::NextFrame()
    {
        m_frameIndex = (m_frameIndex + 1) % m_maxFrameInFlight;
        m_frameCount++;
    }

    RHISwapchain* FrameContext::GetSwapchain()
    {
        return m_swapchain;
    }
}// namespace Wl