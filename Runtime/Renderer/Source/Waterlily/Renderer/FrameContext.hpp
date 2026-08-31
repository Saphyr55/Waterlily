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
#include "Waterlily/RHI/ShaderResourceCache.hpp"
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
        RHICommandAllocator* CommandAllocator;
        RHICommandBuffer* CommandBuffer;

        RHIShaderResourceGroupPool* SRGPool;

        RHISemaphore* FrameAvailableSemaphore;
        RHISemaphore* RenderFinishedSemaphore;
        RHIFence* InFlightFence;

        RenderAllocator UniformAllocator;
        RenderAllocator StorageAllocator;
        UploadScheduler Uploader;
    };

    struct FrameContextInitInfo
    {
        uint32_t FrameWidth;
        uint32_t FrameHeight;
        size_t UniformBufferSize;
        size_t StorageBufferSize;
        size_t StagingBufferSize;
        size_t FrameAllocationSize;
        uint32_t GraphicsCommandBufferCount = 1;
    };

    class WL_RENDERER_API FrameContext
    {
    public:
        static constexpr uint32_t MaxFrameInFlight = 3;

    public:
        void Init(const FrameContextInitInfo& info);
        void Destroy();

        // TODO: This is a temporary solution, we should have a better way to handle shader resource group pool in the
        // future.
        void InitSRGPools();

        void Resize(uint32_t width, uint32_t height);

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        float GetAspectRatio() const;

        FrameResult BeginFrame();
        void EndFrame();

        RHISampler* GetDefaultSampler();

        SharedPtr<RHIDevice> GetDevice() const;
        Frame& GetCurrentFrame();

        uint64_t GetFrameIndex() const;
        uint64_t GetMaxFrameInFlight() const;
        uint64_t GetFrameCount() const;

        RHISwapchain* GetSwapchain();
        SharedPtr<RHIShaderResourceGroupLayoutCache> GetSRGLayoutCache();

    public:
        FrameContext(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
        }
        ~FrameContext() = default;

    private:
        void NextFrame();

    private:
        SharedPtr<RHIDevice> m_device;
        RHISampler* m_defaultSampler = nullptr;

        SharedPtr<RHIShaderResourceGroupLayoutCache> m_srgLayoutCache;

        Array<RHIFence**> m_frameInFlightFences;
        FixedArray<Frame, MaxFrameInFlight> m_frames;

        RHISwapchain* m_swapchain = nullptr;

        uint64_t m_frameIndex = 0;
        uint64_t m_maxFrameInFlight = MaxFrameInFlight;
        uint64_t m_frameCount = 0;
    };


}// namespace Wl
