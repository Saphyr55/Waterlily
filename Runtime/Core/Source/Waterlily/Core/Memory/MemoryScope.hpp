#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    class WL_CORE_API MemoryScope
    {
    public:
        explicit MemoryScope(Allocator* allocator);
        MemoryScope(MemoryScope&&) = delete;
        MemoryScope(const MemoryScope&) = delete;
        ~MemoryScope();
    };

    class WL_CORE_API MemoryStack
    {
        friend MemoryScope;
        
    public:
        static Allocator* GetGlobalAllocator();
        static Allocator* GetCurrentAllocator();
        static Allocator* GetPreviousAllocator();
        static Allocator* GetAllocatorAt(size_t depth);
        
        static void Push(Allocator* allocator);
        static void Pop();

    private:
        static constexpr size_t MaxAllocator = 15; 
        static Allocator* s_allocators[MaxAllocator];
        static size_t s_depth;
    };

    class WL_CORE_API ContextAllocator : public Allocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
        virtual void Deallocate(void* memory, size_t size, size_t alignment = alignof(std::max_align_t)) override;
        
        ContextAllocator();
        ~ContextAllocator() = default;

    private:
        Allocator* m_allocator;
    };

}// namespace Wl