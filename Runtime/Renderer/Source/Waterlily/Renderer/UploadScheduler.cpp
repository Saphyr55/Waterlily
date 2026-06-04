#include "Waterlily/Renderer/UploadScheduler.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"

namespace Wl
{

    void UploadScheduler::Init(const UploadSchedulerInitInfo& info)
    {
        m_device = info.Device;

        RHIBufferDescription description = {};
        description.Size = info.StagingSize;
        description.Usage = RHIBufferUsageFlags::TransferSrc;
        description.MemoryUsage = RHIMemoryUsage::Host;
        description.SharingMode = RHISharingMode::Private;

        RHIBuffer* stagingBuffer = m_device->CreateBuffer(description);

        m_stagingAllocator.Initialize(stagingBuffer, info.MinAlignment);
    }

    void UploadScheduler::Shutdown()
    {
        m_device->DestroyBuffer(static_cast<RHIBuffer*>(m_stagingAllocator.GetBuffer()));
        Reset();
    }

    void UploadScheduler::Reset()
    {
        m_stagingAllocator.Reset();
        m_pendings.Clear();
    }

    RenderAllocation UploadScheduler::Upload(void* data, size_t size, RHIBuffer* dstBuffer, size_t dstOffset)
    {
        RenderAllocation alloc = m_stagingAllocator.Write(data, size);

        BufferUploadRequest request = {};
        request.StagingBuffer = static_cast<RHIBuffer*>(m_stagingAllocator.GetBuffer());
        request.StagingOffset = alloc.Offset;
        request.DstBuffer = dstBuffer;
        request.DstOffset = dstOffset;
        request.Size = size;

        m_pendings.Append(request);

        return alloc;
    }

    void UploadScheduler::Flush(RHICommandBuffer* commandBuffer)
    {
        if (!HasPending())
        {
            return;
        }

        Array<RHIBufferMemoryBarrier> preBarriers;
        preBarriers.Reserve(m_pendings.GetSize());

        for (const BufferUploadRequest& request: m_pendings)
        {
            RHIBufferMemoryBarrier barrier = {};
            barrier.Buffer = request.DstBuffer;
            barrier.Offset = request.DstOffset;
            barrier.SourceUsage = request.DstBuffer->GetUsage();
            barrier.DestinationUsage = RHIBufferUsageFlags::TransferDst;
            barrier.Size = request.Size;

            preBarriers.Append(barrier);
        }

        commandBuffer->PipelineBarrier(preBarriers);

        for (const BufferUploadRequest& request: m_pendings)
        {
            RHICopyBufferCommand cmd = {};
            cmd.Source = request.StagingBuffer;
            cmd.SourceOffset = request.StagingOffset;
            cmd.Destination = request.DstBuffer;
            cmd.DestinationOffset = request.DstOffset;
            cmd.Size = request.Size;

            commandBuffer->CopyBuffer(cmd);
        }

        Array<RHIBufferMemoryBarrier> postBarriers;
        postBarriers.Reserve(m_pendings.GetSize());

        for (const BufferUploadRequest& request: m_pendings)
        {
            RHIBufferMemoryBarrier barrier = {};
            barrier.Buffer = request.DstBuffer;
            barrier.Offset = request.DstOffset;
            barrier.Size = request.Size;
            barrier.SourceUsage = RHIBufferUsageFlags::TransferDst;
            barrier.DestinationUsage = request.DstBuffer->GetUsage();

            postBarriers.Append(barrier);
        }

        commandBuffer->PipelineBarrier(postBarriers);
    }

}// namespace Wl