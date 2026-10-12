#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/Core/Memory/MemoryPool.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/DeviceFactory.hpp"
#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"

namespace Wl
{

    class RHIDevice;

    enum class FrameResult
    {
        Success = 0,
        Unknown = 1,
        OutOfDate = 2,
    };

    struct Frame
    {
        RHICommandAllocator* CommandAllocator = nullptr;
        RHICommandBuffer* CommandBuffer = nullptr;

        RHIShaderResourceGroupPool* SRGPool = nullptr;

        RHISemaphore* FrameAvailableSemaphore = nullptr;
        Array<RHISemaphore*> RenderFinishedSemaphore;
        RHIFence* InFlightFence = nullptr;

        RenderAllocator UniformAllocator;
        RenderAllocator StorageAllocator;
        UploadScheduler Uploader;
    };

    struct FrameContextInitInfo
    {
        uint32 FrameWidth;
        uint32 FrameHeight;
        usize UniformBufferSize;
        usize StorageBufferSize;
        usize StagingBufferSize;
        usize FrameAllocationSize;
        uint32 GraphicsCommandBufferCount = 1;
    };

    class WL_RENDERER_API FrameContext
    {
    public:
        static constexpr uint32 MaxFrameInFlight = 3;

    public:
        void Init(const FrameContextInitInfo& info);
        void Destroy();

        // TODO: This is a temporary solution, we should have a better way to handle shader resource group pool in the
        // future.
        void InitSRGPools();

        void Resize(uint32 width, uint32 height);

        uint32 GetWidth() const;
        uint32 GetHeight() const;
        float GetAspectRatio() const;

        FrameResult BeginFrame();
        void EndFrame();

        SharedPtr<RHIDevice> GetDevice() const;
        Frame& GetCurrentFrame();

        ArrayView<Frame> GetFrames() const
        {
            return m_frames;
        }

        uint64 GetFrameIndex() const;
        uint64 GetMaxFrameInFlight() const;
        uint64 GetFrameCount() const;

        RHISwapchain* GetSwapchain();

    private:
        void NextFrame();

    public:
        FrameContext(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
        }
        ~FrameContext() = default;

    private:
        SharedPtr<RHIDevice> m_device;

        Array<RHIFence**> m_frameInFlightFences;
        FixedArray<Frame, MaxFrameInFlight> m_frames;

        RHISwapchain* m_swapchain = nullptr;

        uint64 m_frameIndex = 0;
        uint64 m_maxFrameInFlight = MaxFrameInFlight;
        uint64 m_frameCount = 0;
    };


}// namespace Wl
