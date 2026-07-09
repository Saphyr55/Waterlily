#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    class WL_CORE_API DefaultAllocator : public Allocator
    {
    public:
        static Allocator& GetDefault();

        virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
        virtual void Deallocate(void* memory, size_t size, size_t alignment = alignof(std::max_align_t)) override;

        DefaultAllocator() = default;
        ~DefaultAllocator() = default;
    };

}// namespace Wl
