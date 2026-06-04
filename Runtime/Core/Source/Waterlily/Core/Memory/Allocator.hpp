#pragma once

#include <cstdint>

namespace Wl
{

    class Allocator
    {
    public:
        virtual void* Allocate(size_t size) = 0;
        virtual void Deallocate(void* memory, size_t size) = 0;
    };

    class AlignedAllocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment) = 0;
        virtual void Deallocate(void* memory, size_t size, size_t alignment) = 0;
    };

}// namespace Wl
