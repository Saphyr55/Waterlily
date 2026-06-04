#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"
#include "Waterlily/Core/Memory/TypedAllocator.hpp"

namespace Wl
{

    class WL_CORE_API DefaultAllocator : public HeapAllocator
    {
    public:
        static DefaultAllocator& GetInstance();
    };

    template<typename Type>
    class DefaultTypedAllocator : public TypedAllocator<Type, DefaultAllocator>
    {
        using Super = TypedAllocator<Type, DefaultAllocator>;

    public:
        DefaultTypedAllocator()
            : Super(&DefaultAllocator::GetInstance())
        {
        }
    };

}// namespace Wl
