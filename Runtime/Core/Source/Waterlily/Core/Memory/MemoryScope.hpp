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
        static Allocator* GetAllocatorAt(usize depth);

        static void Push(Allocator* allocator);
        static void Pop();

    private:
        static constexpr usize MaxAllocator = 15;
        static Allocator* s_allocators[MaxAllocator];
        static usize s_depth;
    };

    class WL_CORE_API ContextAllocator : public Allocator
    {
    public:
        virtual void* Allocate(usize size, usize alignment = alignof(std::max_align_t)) override;
        virtual void Deallocate(void* memory, usize size, usize alignment = alignof(std::max_align_t)) override;

        ContextAllocator();
        ~ContextAllocator() = default;

    private:
        Allocator* m_allocator;
    };

}// namespace Wl