#pragma once

#include "Waterlily/Core/Object/Variable.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl 
{
    
    struct MemberInfo
    {
        StringID name;
        Variable variable;
        uint32_t offset;
        uint32_t size;
        uint32_t align;

        // For the ordered set.
        inline constexpr bool operator<(const MemberInfo& rhs) const
        {
            return offset < rhs.offset;
        }
    };
    
}