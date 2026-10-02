#pragma once

#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    class HeapAllocator : public Allocator
    {
    public:
        inline void* Allocate(usize size)
        {
            return Memory::Allocate(size);
        }

        inline void Deallocate(void* memory, usize size)
        {
            Memory::Deallocate(memory, size);
        }

        inline virtual void* Allocate(usize size, usize alignment) override
        {
            return Memory::Allocate(size, alignment);
        }

        inline virtual void Deallocate(void* memory, usize size, usize alignment) override
        {
            Memory::Deallocate(memory, size, alignment);
        }
    };

}// namespace Wl