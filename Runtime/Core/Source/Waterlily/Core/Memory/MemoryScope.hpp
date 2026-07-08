#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    class WL_CORE_API MemoryScope
    {
    public:
        static inline Allocator* GetCurrentAllocator()
        {
            return s_currentAllocator;
        }

        inline Allocator* GetPreviousAllocator()
        {
            return m_previousAllocator;
        }

    public:
        MemoryScope(Allocator* newAllocator)
        {
            m_previousAllocator = s_currentAllocator;
            s_currentAllocator = newAllocator;
        }

        ~MemoryScope()
        {
            s_currentAllocator = m_previousAllocator;
        }

    private:
        static inline thread_local Allocator* s_currentAllocator;
        Allocator* m_previousAllocator;
    };

}// namespace Wl