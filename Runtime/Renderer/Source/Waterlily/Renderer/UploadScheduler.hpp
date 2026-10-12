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
        usize StagingOffset;

        RHIBuffer* DstBuffer;
        usize DstOffset;

        usize Size;
    };

    struct UploadSchedulerInitInfo
    {
        SharedPtr<RHIDevice> Device;
        usize StagingSize;
        usize MinAlignment;
    };

    class WL_RENDERER_API UploadScheduler
    {
    public:
        void Init(const UploadSchedulerInitInfo& info);
        void Shutdown();

        void Reset();

        RenderAllocation Upload(void* data, usize size, RHIBuffer* dstBuffer, usize dstOffset = 0);

        template<typename DataType>
        RenderAllocation Upload(ArrayView<DataType> data, RHIBuffer* dstBuffer, usize dstOffset = 0);

        void Flush(RHICommandBuffer* cmd);

        inline bool HasPending() const;

        inline usize GetTotalPendingBytes() const;

    public:
        UploadScheduler() = default;
        ~UploadScheduler() = default;

    private:
        SharedPtr<RHIDevice> m_device;
        RenderAllocator m_stagingAllocator;
        Array<BufferUploadRequest> m_pendings;
    };

    inline usize UploadScheduler::GetTotalPendingBytes() const
    {
        return m_stagingAllocator.GetHead();
    }

    inline bool UploadScheduler::HasPending() const
    {
        return !m_pendings.IsEmpty();
    }

    template<typename DataType>
    RenderAllocation UploadScheduler::Upload(ArrayView<DataType> data, RHIBuffer* dstBuffer, usize dstOffset)
    {
        return Upload(data.GetData(), data.GetSizeInBytes(), dstBuffer, dstOffset);
    }

}// namespace Wl