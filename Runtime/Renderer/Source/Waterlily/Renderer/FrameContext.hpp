#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/RHI/BindlessShaderResources.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Semaphore.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/UploadScheduler.hpp"
#include <cstdint>

namespace Wl
{

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
        uint32_t GraphicsCommandBufferCount = 1;
    };

    class WL_RENDERER_API FrameContext
    {
    public:
        static constexpr uint32_t MaxFrameInFlight = 3;

    public:
        void Init(const FrameContextInitInfo& info);
        void Shutdown();

        // TODO: This is a temporary solution, we should have a better way to handle shader resource group pool in the
        // future.
        void InitializeSRGPools();

        void Resize(uint32_t width, uint32_t height);

        FrameResult BeginFrame();
        void EndFrame();

        inline Frame& GetCurrentFrame();
        inline void NextFrame();

        inline uint64_t GetFrameIndex();
        inline uint64_t GetMaxFrameInFlight();
        inline uint64_t GetFrameCount();

        inline RHISwapchain* GetSwapchain();
        inline SharedPtr<RHIShaderResourceGroupLayoutCache> GetSRGLayoutCache();

    public:
        FrameContext() = default;
        ~FrameContext() = default;

    private:
        SharedPtr<RHIDevice> m_device;

        SharedPtr<RHIShaderResourceGroupLayoutCache> m_srgLayoutCache;

        Array<RHIFence**> m_frameInFlightFences;
        FixedArray<Frame, MaxFrameInFlight> m_frames;

        RHISwapchain* m_swapchain = nullptr;

        uint64_t m_frameIndex = 0;
        uint64_t m_maxFrameInFlight = MaxFrameInFlight;
        uint64_t m_frameCount = 0;
    };

    inline SharedPtr<RHIShaderResourceGroupLayoutCache> FrameContext::GetSRGLayoutCache()
    {
        return m_srgLayoutCache;
    }

    inline uint64_t FrameContext::GetFrameIndex()
    {
        return m_frameIndex;
    }

    inline uint64_t FrameContext::GetMaxFrameInFlight()
    {
        return m_maxFrameInFlight;
    }

    inline uint64_t FrameContext::GetFrameCount()
    {
        return m_frameCount;
    }

    inline Frame& FrameContext::GetCurrentFrame()
    {
        return m_frames[m_frameIndex];
    }

    inline void FrameContext::NextFrame()
    {
        m_frameIndex = (m_frameIndex + 1) % m_maxFrameInFlight;
        m_frameCount++;
    }

    inline RHISwapchain* FrameContext::GetSwapchain()
    {
        return m_swapchain;
    }

}// namespace Wl
