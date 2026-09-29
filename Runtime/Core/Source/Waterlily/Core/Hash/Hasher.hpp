#pragma once

#include <cstdint>
#include <functional>

namespace Wl
{

    template<typename Type>
    using Hasher = std::hash<Type>;

    template<typename T>
    inline uint64_t Hash(const T& value)
    {
        return ::Wl::Hasher<T> {}(value);
    }
    
    inline uint64_t HashCombine(uint64_t seed, uint64_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }

}// namespace Wl

#define WL_HASH_DEFINE(TYPE, VAR_NAME, BODY)                              \
    namespace std                                                         \
    {                                                                     \
        template<>                                                        \
        struct hash<TYPE>                                                 \
        {                                                                 \
            uint64_t operator()(const TYPE& VAR_NAME) const noexcept BODY \
        };                                                                \
    }

#define WL_HASH_TEMPLATED_DEFINE(TEMPLATE_TYPE, TYPE, VAR_NAME, BODY)                  \
    namespace std                                                                      \
    {                                                                                  \
        template<typename TEMPLATE_TYPE>                                               \
        struct hash<TYPE<TEMPLATE_TYPE>>                                               \
        {                                                                              \
            size_t operator()(const TYPE<TEMPLATE_TYPE>& VAR_NAME) const noexcept BODY \
        };                                                                             \
    }
