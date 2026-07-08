#pragma once

#include <concepts>

namespace Wl
{

    template<typename AllocatorType>
    concept CAllocator = requires(AllocatorType allocator, void* resource, size_t n, size_t alignment) 
    {
        { allocator.Allocate(n, alignment) } -> std::same_as<void*>;
        { allocator.Deallocate(resource, n, alignment) } -> std::same_as<void>;
    };

}// namespace Wl
