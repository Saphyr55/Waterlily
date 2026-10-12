#include "Waterlily/Core/Memory/StackAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    StackAllocator::StackAllocator(usize size)
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

    void StackAllocator::Initialize(usize size)
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

    void* StackAllocator::Allocate(usize size, usize alignment)
    {
        uint8* currentAddress = m_buffer + m_head;
        uint8* alignedAddress = Memory::Align(currentAddress, alignment);

        usize padding = alignedAddress - currentAddress;
        usize totalSize = size + padding;

        if (m_head + totalSize > m_size)
        {
            // Not enough space.
            return nullptr;
        }

        m_head += totalSize;

        return alignedAddress;
    }

    void StackAllocator::Deallocate(void* /* memory */, usize /* size */, usize /* alignment */)
    {
        // Stack allocator does not support deallocation of individual blocks.
        // Deallocation is done by resetting the entire allocator.
    }

}// namespace Wl