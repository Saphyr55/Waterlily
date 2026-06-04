#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    // See: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    constexpr uint64_t fnv1a(const uint8_t* data, size_t length)
    {
        uint64_t FNV_offsetBasis = 0xcbf29ce484222325;
        uint64_t FNV_prime = 0x100000001b3;

        uint64_t hash = FNV_offsetBasis;

        for (size_t i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }

    constexpr uint64_t fnv1a_cstr(const char* data, size_t length)
    {
        uint64_t FNV_offsetBasis = 0xcbf29ce484222325;
        uint64_t FNV_prime = 0x100000001b3;

        uint64_t hash = FNV_offsetBasis;

        for (size_t i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }

    constexpr uint64_t fnv1a_cstr(const wchar_t* data, size_t length)
    {
        uint64_t FNV_offsetBasis = 0xcbf29ce484222325;
        uint64_t FNV_prime = 0x100000001b3;

        uint64_t hash = FNV_offsetBasis;

        for (size_t i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }

}// namespace Wl
