#pragma once

#include "Waterlily/RHI/RHIExports.hpp"

namespace Wl
{

    class Allocator;
    class RHIBuffer;
    struct RHIBufferDescription;

    class WL_RHI_API RHIBufferPool
    {
    public:
        virtual void InitPool() = 0;

        virtual RHIBuffer* Allocate(const RHIBufferDescription& description) = 0;

        virtual void Deallocate(RHIBuffer* buffer) = 0;

        virtual void Reset() = 0;

        virtual ~RHIBufferPool() = default;
    };

}// namespace Wl