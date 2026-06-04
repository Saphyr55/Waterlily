#pragma once

#include "Waterlily/Core/Memory/MemoryPool.hpp"
#include "Waterlily/RHI/BufferPool.hpp"
#include "Waterlily/RHIVulkan/VulkanBuffer.hpp"

namespace Wl
{

    class VulkanBufferPool : public RHIBufferPool
    {
    public:
        virtual void InitPool() override;

        virtual RHIBuffer* Allocate(const RHIBufferDescription& description) override;

        virtual void Deallocate(RHIBuffer* buffer) override;

        virtual void Reset() override;

    public:
        VulkanBufferPool() = default;

        virtual ~VulkanBufferPool() override = default;

    private:
        void DestroyBuffer(VulkanBuffer* vulkanBuffer);

    private:
        MemoryPool<VulkanBuffer> m_pool;
    };

}// namespace Wl