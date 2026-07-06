#pragma once

#include <cstddef>

namespace Wl
{

    class Allocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
        virtual void Deallocate(void* memory, size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
    };

}// namespace Wl
