#include "Waterlily/Core/Identifiers/UUID.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <cstdio>
#include <random>

namespace Wl
{

    thread_local static std::random_device rd;
    thread_local static std::mt19937_64 gen(rd());
    thread_local static std::uniform_int_distribution<uint64_t> dis64;

    UUID_64 UUID_64::Generate()
    {
        return UUID_64();
    }

    UUID_64 UUID_64::CreateInvalid()
    {
        return UUID_64(0);
    }

    UUID_64::UUID_64()
        : m_value(dis64(gen))
    {
    }

    String UUID_64::ToString() const
    {
        FixedArray<char, 16 + 1> buffer;
        snprintf(buffer.GetData(), buffer.GetSizeInBytes(), "%016llx", m_value);
        return String(buffer.GetData());
    }

}// namespace Wl
