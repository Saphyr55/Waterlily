#pragma once

#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/TextureView.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHISwapchainBuffer
    {
        RHITexture* Texture;
        RHITextureView* View;
    };

    struct RHISwapchainAcquireResult
    {
        uint32_t ImageIndex = 0;
        bool IsSuccess = false;
        bool IsOutOfDate = false;
        bool IsSuboptimal = false;
        bool IsNotReady = false;
    };

    class RHISwapchain
    {
    public:
        virtual RHISwapchainAcquireResult& AcquireNextFrame(RHISemaphore* semaphore) = 0;

        virtual RHISwapchainAcquireResult& GetCurrentAcquireResult() = 0;

        virtual uint32_t GetWidth() = 0;

        virtual uint32_t GetHeight() = 0;

        virtual RHIFormat GetFormat() = 0;

        virtual ArrayView<RHISwapchainBuffer> GetBuffers() const = 0;

        virtual const RHISwapchainBuffer& GetCurrentBuffer() const = 0;
    };

}// namespace Wl