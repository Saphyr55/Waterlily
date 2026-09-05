#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"

namespace Wl
{

    static HeapAllocator s_globalAllocator;

    size_t MemoryStack::s_depth = 0;
    Allocator* MemoryStack::s_allocators[MemoryStack::MaxAllocator] = {&s_globalAllocator};
    
    Allocator* MemoryStack::GetGlobalAllocator()
    {
        return &s_globalAllocator;
    }

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

    void* ContextAllocator::Allocate(size_t size, size_t alignment)
    {
        return m_allocator->Allocate(size, alignment);
    }

    void ContextAllocator::Deallocate(void* memory, size_t size, size_t alignment)
    {
        m_allocator->Deallocate(memory, size, alignment);
    }

    ContextAllocator::ContextAllocator()
    {
        m_allocator = MemoryStack::GetCurrentAllocator();
    }

}// namespace Wl
