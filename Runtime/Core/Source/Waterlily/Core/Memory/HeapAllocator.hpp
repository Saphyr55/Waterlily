#pragma once

#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    class HeapAllocator : public Allocator
    {
    public:
        inline void* Allocate(size_t size)
        {
            return Memory::Allocate(size);
        }

        inline void Deallocate(void* memory, size_t size)
        {
            Memory::Deallocate(memory, size);
        }

        inline virtual void* Allocate(size_t size, size_t alignment) override
        {
            return Memory::Allocate(size, alignment);
        }

        inline virtual void Deallocate(void* memory, size_t size, size_t alignment) override
        {
            Memory::Deallocate(memory, size, alignment);
        }
    };

}// namespace Wl