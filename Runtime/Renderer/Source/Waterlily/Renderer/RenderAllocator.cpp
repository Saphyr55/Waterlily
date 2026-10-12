#include "Waterlily/Renderer/RenderAllocator.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    RenderAllocation RenderAllocator::Allocate(usize size)
    {
        uintptr_t offset = Memory::AlignUp(m_head, m_minAlignment);
        uintptr_t newHead = offset + size;
        uintptr_t bufferHead = m_buffer->GetSize();

        if (newHead > bufferHead)
        {
            WL_CRASH("RenderAllocation overflow.");
            return RenderAllocation{};
        }

        m_head = newHead;

        void* data = static_cast<uint8*>(m_mapped) + offset;

        return RenderAllocation{
                .Buffer = m_buffer,
                .Data = data,
                .Offset = offset,
                .Size = size,
        };
    }

    RenderAllocation RenderAllocator::Write(const void* data, usize size)
    {
        RenderAllocation allocation = Allocate(size);
        Update(allocation, data);
        return allocation;
    }

    void RenderAllocator::Update(RenderAllocation& allocation, const void* data)
    {
        Memory::Copy(allocation.Data, data, allocation.Size);
    }

}// namespace Wl
