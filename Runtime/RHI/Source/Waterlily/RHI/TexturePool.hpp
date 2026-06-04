#pragma once

#include "Waterlily/RHI/RHIExports.hpp"

namespace Wl
{

    class Allocator;
    class RHITexture;
    struct RHITextureDescription;

    class WL_RHI_API RHITexturePool
    {
    public:
        virtual RHITexture* Allocate(const RHITextureDescription& description) = 0;

        virtual void Deallocate(RHITexture* texture) = 0;

        virtual void Reset() = 0;

        virtual ~RHITexturePool() = default;
    };

}// namespace Wl