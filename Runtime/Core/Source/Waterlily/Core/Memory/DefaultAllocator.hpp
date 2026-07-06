#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"

namespace Wl
{

    class WL_CORE_API DefaultAllocator : public HeapAllocator
    {
    public:
        static DefaultAllocator& GetInstance();
    };

}// namespace Wl
