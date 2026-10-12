#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    // See: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    constexpr uint64 fnv1a(const uint8* data, usize length)
    {
        uint64 FNV_offsetBasis = 0xcbf29ce484222325;
        uint64 FNV_prime = 0x100000001b3;

        uint64 hash = FNV_offsetBasis;

        for (usize i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }

    constexpr uint64 fnv1a_cstr(const char* data, usize length)
    {
        uint64 FNV_offsetBasis = 0xcbf29ce484222325;
        uint64 FNV_prime = 0x100000001b3;

        uint64 hash = FNV_offsetBasis;

        for (usize i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }
    
    template<usize N>
    constexpr uint64 fnv1a_cstr(const char (&data)[N])
    {
        return fnv1a_cstr(data, N);
    }

    constexpr uint64 fnv1a_cstr(const wchar_t* data, usize length)
    {
        uint64 FNV_offsetBasis = 0xcbf29ce484222325;
        uint64 FNV_prime = 0x100000001b3;

        uint64 hash = FNV_offsetBasis;

        for (usize i = 0; i < length; i++)
        {
            hash ^= data[i];
            hash *= FNV_prime;
        }

        return hash;
    }

}// namespace Wl
