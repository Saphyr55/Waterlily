#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    void* LinearAllocator::Allocate(const size_t size, size_t alignment)
    {
        size_t space = m_size - m_offset;
        uint8_t* block = m_buffer + m_offset;
        uint8_t* alignedBlock = Memory::Align(block, alignment);

        if (!alignedBlock || alignedBlock + size > m_buffer + m_size)
        {
            // Not enough space.
            return nullptr;
        }

        m_offset = alignedBlock - m_buffer + size;

        return alignedBlock;
    }

    void LinearAllocator::Deallocate(void* /* block */, size_t /* size */, size_t /* alignment */)
    {
        // Linear allocator does not support deallocation of individual blocks.
        // Deallocation is done by resetting the entire allocator.
    }

    void LinearAllocator::Destroy()
    {
        if (m_buffer)
        {
            return;
        }

        m_parent->Deallocate(m_buffer, m_size);
        m_buffer = nullptr;
        m_size = 0;
        m_offset = 0;
    }

    void LinearAllocator::Reset()
    {
        m_offset = 0;
    }

    LinearAllocator::LinearAllocator(Allocator* parent, size_t size)
        : m_parent(parent)
        , m_size(size)
        , m_offset(0)
        , m_buffer(nullptr)
    {
        WL_CHECK(parent);

        if (m_buffer)
        {
            Destroy();
        }
        m_buffer = reinterpret_cast<uint8_t*>(m_parent->Allocate(m_size));
        if (m_buffer)
        {
            Memory::Write(m_buffer, 0, m_size);
        }
    }

    LinearAllocator::~LinearAllocator()
    {
        if (m_buffer)
        {
            Destroy();
        }
    }

}// namespace Wl