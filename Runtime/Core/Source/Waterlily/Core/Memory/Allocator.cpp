#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"

namespace Wl
{

    void* DefaultAllocator::Allocate(size_t size, size_t alignment)
    {
        return m_allocator->Allocate(size, alignment);
    }

    void DefaultAllocator::Deallocate(void* memory, size_t size, size_t alignment)
    {
        m_allocator->Deallocate(memory, size, alignment);
    }

    DefaultAllocator::DefaultAllocator()
    {
        m_allocator = MemoryStack::GetCurrentAllocator();
    }

}// namespace Wl