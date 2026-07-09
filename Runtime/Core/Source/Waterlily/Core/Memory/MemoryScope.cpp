#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"

namespace Wl
{
    static HeapAllocator s_heap;

    Allocator* MemoryStack::GetCurrentAllocator()
    {
        return GetAllocatorAt(m_depth);
    }

    Allocator* MemoryStack::GetPreviousAllocator()
    {
        if (m_depth == 0)
        {
            return nullptr;
        }

        return GetAllocatorAt(m_depth - 1);
    }

    Allocator* MemoryStack::GetAllocatorAt(size_t depth)
    {
        return m_allocators[depth];
    }

    MemoryStack::MemoryStack()
    {
        m_allocators[m_depth] = &s_heap;
    }

    MemoryStack& MemoryStack::GetInstance()
    {
        static thread_local MemoryStack s_stack;
        return s_stack;
    }

    MemoryScope::MemoryScope(Allocator* allocator)
    {
        MemoryControl control;
        control.Open(allocator);
    }

    MemoryScope::~MemoryScope()
    {
        MemoryControl control;
        control.Close();
    }

    void MemoryControl::Open(Allocator* allocator)
    {
        MemoryStack& stack = MemoryStack::GetInstance();
        stack.m_allocators[++stack.m_depth] = allocator;
    }

    void MemoryControl::Close()
    {
        MemoryStack& stack = MemoryStack::GetInstance();
        stack.m_depth--;
    }

}// namespace Wl
