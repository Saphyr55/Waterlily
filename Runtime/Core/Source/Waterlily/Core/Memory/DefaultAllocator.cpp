
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"

namespace Wl
{

    DefaultAllocator& DefaultAllocator::GetInstance()
    {
        static DefaultAllocator s_defaultAllocator;
        return s_defaultAllocator;
    }

}// namespace Wl