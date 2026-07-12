#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"

namespace Wl
{

    static HeapAllocator s_heap;

    size_t MemoryStack::s_depth = 0;
    Allocator* MemoryStack::s_allocators[MemoryStack::MaxAllocator] = {&s_heap};

    Allocator* MemoryStack::GetCurrentAllocator()
    {
        return GetAllocatorAt(s_depth);
    }

    Allocator* MemoryStack::GetPreviousAllocator()
    {
        if (s_depth == 0)
        {
            return nullptr;
        }

        return GetAllocatorAt(s_depth - 1);
    }

    Allocator* MemoryStack::GetAllocatorAt(size_t depth)
    {
        return s_allocators[depth];
    }

    MemoryScope::MemoryScope(Allocator* allocator)
    {
        MemoryStack::Push(allocator);
    }

    MemoryScope::~MemoryScope()
    {
        MemoryStack::Pop();
    }

    void MemoryStack::Push(Allocator* allocator)
    {
        WL_CHECK(MemoryStack::s_depth + 1 < MemoryStack::MaxAllocator);

        MemoryStack::s_depth++;
        MemoryStack::s_allocators[MemoryStack::s_depth] = allocator;
    }

    void MemoryStack::Pop()
    {
        WL_CHECK(MemoryStack::s_depth > 0);

        MemoryStack::s_depth--;
    }

}// namespace Wl
