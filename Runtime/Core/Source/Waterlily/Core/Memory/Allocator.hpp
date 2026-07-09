#pragma once

#include "Waterlily/Core/CoreExports.hpp"

#include <cstddef>

namespace Wl
{

    class Allocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
        virtual void Deallocate(void* memory, size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
    };
    
    class WL_CORE_API DefaultAllocator : public Allocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
        virtual void Deallocate(void* memory, size_t size, size_t alignment = alignof(std::max_align_t)) override;
        
        DefaultAllocator();
        ~DefaultAllocator() = default;

    private:
        Allocator* m_allocator;
    };

}// namespace Wl
