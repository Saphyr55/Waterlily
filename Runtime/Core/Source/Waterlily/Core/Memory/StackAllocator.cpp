#include "Waterlily/Core/Memory/StackAllocator.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    StackAllocator::StackAllocator(size_t size)
        : m_size(size)
        , m_head(0)
        , m_buffer(nullptr)
    {
        Initialize(size);
    }

    StackAllocator::~StackAllocator()
    {
        Destroy();
    }

    void StackAllocator::Initialize(size_t size)
    {
        m_buffer = Memory::Allocate(size);
        m_size = size;
        m_head = 0;
    }

    void StackAllocator::Destroy()
    {
        Memory::Deallocate(m_buffer, m_size);
        m_buffer = nullptr;
        m_size = 0;
        m_head = 0;
    }

    void* StackAllocator::Allocate(size_t size, size_t alignment)
    {
        uint8_t* currentAddress = m_buffer + m_head;
        uint8_t* alignedAddress = Memory::Align(currentAddress, alignment);

        size_t padding = alignedAddress - currentAddress;
        size_t totalSize = size + padding;

        if (m_head + totalSize > m_size)
        {
            // Not enough space.
            return nullptr;
        }

        m_head += totalSize;

        return alignedAddress;
    }

    void StackAllocator::Deallocate(void* /* memory */, size_t /* size */, size_t /* alignment */)
    {
        // Stack allocator does not support deallocation of individual blocks.
        // Deallocation is done by resetting the entire allocator.
    }

}// namespace Wl