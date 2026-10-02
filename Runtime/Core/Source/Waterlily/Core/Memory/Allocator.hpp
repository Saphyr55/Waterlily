#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class Allocator
    {
    public:
        virtual void* Allocate(usize size, usize alignment = alignof(std::max_align_t)) = 0;
        virtual void Deallocate(void* memory, usize size, usize alignment = alignof(std::max_align_t)) = 0;
    };

}// namespace Wl
