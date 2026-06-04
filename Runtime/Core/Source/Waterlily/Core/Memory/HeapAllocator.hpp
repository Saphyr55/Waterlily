#pragma once

#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/TypedAllocator.hpp"

namespace Wl
{

    class HeapAllocator : public AlignedAllocator
        , public Allocator
    {
    public:
        inline virtual void* Allocate(size_t size) override
        {
            return Memory::Allocate(size);
        }

        inline virtual void Deallocate(void* memory, size_t size) override
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

        HeapAllocator() = default;
        ~HeapAllocator() = default;
    };

    template<typename Type>
    using TypedHeapAllocator = TypedAllocator<Type, HeapAllocator>;

}// namespace Wl