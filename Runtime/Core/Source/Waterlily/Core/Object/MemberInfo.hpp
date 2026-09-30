#pragma once

#include "Waterlily/Core/Function/Method.hpp"
#include "Waterlily/Core/Object/Variable.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    struct MemberInfo
    {
        StringID name;
        Type owner;
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

    struct MethodInfo
    {
        StringID name;
        Type owner;
        Type signature;
        Variable returnVar;
        Array<Variable> paramVars;
        MethodHandle handle;
    };

}// namespace Wl