#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    enum class Ordering : int8_t
    {
        LESS = -1,
        EQUAL = 0,
        GREATER = 1
    };

    class Comparator
    {
        template<typename T>
        static Ordering Compare(const T& lhs, const T& rhs)
        {
            if (lhs < rhs)
            {
                return Ordering::LESS;
            }

            if (lhs > rhs)
            {
                return Ordering::GREATER;
            }

            return Ordering::EQUAL;
        }
    };

}// namespace Wl