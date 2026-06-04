#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    struct BufferUploadRequest
    {
        RHIBuffer* StagingBuffer;
        size_t StagingOffset;

        RHIBuffer* DstBuffer;
        size_t DstOffset;

        size_t Size;
    };

    struct UploadSchedulerInitInfo
    {
        SharedPtr<RHIDevice> Device;
        size_t StagingSize;
        size_t MinAlignment;
    };

    class WL_RENDERER_API UploadScheduler
    {
    public:
        void Init(const UploadSchedulerInitInfo& info);
        void Shutdown();

        void Reset();

        RenderAllocation Upload(void* data, size_t size, RHIBuffer* dstBuffer, size_t dstOffset = 0);

        template<typename DataType>
        RenderAllocation Upload(ArrayView<DataType> data, RHIBuffer* dstBuffer, size_t dstOffset = 0);

        void Flush(RHICommandBuffer* cmd);

        inline bool HasPending() const;

        inline size_t GetTotalPendingBytes() const;

    public:
        UploadScheduler() = default;
        ~UploadScheduler() = default;

    private:
        SharedPtr<RHIDevice> m_device;
        RenderAllocator m_stagingAllocator;
        Array<BufferUploadRequest> m_pendings;
    };

    inline size_t UploadScheduler::GetTotalPendingBytes() const
    {
        return m_stagingAllocator.GetHead();
    }

    inline bool UploadScheduler::HasPending() const
    {
        return !m_pendings.IsEmpty();
    }

    template<typename DataType>
    RenderAllocation UploadScheduler::Upload(ArrayView<DataType> data, RHIBuffer* dstBuffer, size_t dstOffset)
    {
        return Upload(data.GetData(), data.GetSizeInBytes(), dstBuffer, dstOffset);
    }

}// namespace Wl