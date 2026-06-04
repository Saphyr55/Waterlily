#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

#include <functional>

namespace Wl
{
    template<typename Type>
    using Hash = std::hash<Type>;

    class WL_CORE_API Hasher
    {
    public:
        template<typename T>
        static size_t hash(const T& value)
        {
            return Hash<T>{}(value);
        }
    };

    inline size_t HashCombine(size_t seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }

}// namespace Wl

#define WL_HASH_DEFINE(TYPE, VAR_NAME, BODY)                            \
    namespace std                                                       \
    {                                                                   \
        template<>                                                      \
        struct hash<TYPE>                                               \
        {                                                               \
            size_t operator()(const TYPE& VAR_NAME) const noexcept BODY \
        };                                                              \
    }

#define WL_HASH_TEMPLATED_DEFINE(TEMPLATE_TYPE, TYPE, VAR_NAME, BODY)              \
    namespace std template<typename TEMPLATE_TYPE>                                 \
    struct ::std::hash<TYPE<TEMPLATE_TYPE>>                                        \
    {                                                                              \
        size_t operator()(const TYPE<TEMPLATE_TYPE>& VAR_NAME) const noexcept BODY \
    };                                                                             \
    }
