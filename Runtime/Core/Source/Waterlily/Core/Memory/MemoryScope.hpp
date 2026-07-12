#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include <cstdint>

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

}// namespace Wl