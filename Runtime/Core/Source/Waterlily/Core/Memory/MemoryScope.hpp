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

    class WL_CORE_API MemoryControl
    {
    public:
        void Open(Allocator* allocator);
        void Close();
        
        MemoryControl() = default;
        ~MemoryControl() = default;
    };

    class WL_CORE_API MemoryStack
    {
        friend MemoryScope;
        friend MemoryControl;
        
    public:
        static MemoryStack& GetInstance();

        Allocator* GetCurrentAllocator();
        Allocator* GetPreviousAllocator();
        Allocator* GetAllocatorAt(size_t depth);

        MemoryStack();
        ~MemoryStack() = default;

    private:
        Allocator* m_allocators[15];
        size_t m_depth = 0;
    };

}// namespace Wl