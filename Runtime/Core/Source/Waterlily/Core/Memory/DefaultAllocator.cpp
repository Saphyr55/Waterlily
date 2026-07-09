
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"

namespace Wl
{

    thread_local static HeapAllocator s_heap;

    void* DefaultAllocator::Allocate(size_t size, size_t alignment)
    {
        return GetDefault().Allocate(size, alignment);
    }

    void DefaultAllocator::Deallocate(void* memory, size_t size, size_t alignment)
    {
        GetDefault().Deallocate(memory, size, alignment);
    }

    Allocator& DefaultAllocator::GetDefault()
    {
        return s_heap;
    }

}// namespace Wl