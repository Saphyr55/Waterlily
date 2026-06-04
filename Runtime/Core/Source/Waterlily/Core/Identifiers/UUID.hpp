#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/String/String.hpp"

namespace Wl
{

    class WL_CORE_API UUID_64
    {
    public:
        static UUID_64 Generate();
        static UUID_64 CreateInvalid();

        UUID_64();
        explicit UUID_64(uint64_t value)
            : m_value(value)
        {
        }

        UUID_64(const UUID_64& other) = default;
        UUID_64(UUID_64&&) noexcept = default;

        UUID_64& operator=(const UUID_64& other) = default;
        UUID_64& operator=(UUID_64&& other) noexcept = default;

        bool operator==(const UUID_64& other) const
        {
            return m_value == other.m_value;
        }

        bool operator!=(const UUID_64& other) const
        {
            return m_value != other.m_value;
        }

        bool IsValid() const
        {
            return m_value != 0;
        }

        uint64_t GetValue() const
        {
            return m_value;
        }

        String ToString() const;

    private:
        uint64_t m_value = 0;
    };

    inline void operator<<(OutputStream& stream, const UUID_64& UUID)
    {
        stream << UUID.GetValue();
    }

    inline void operator>>(InputStream& stream, UUID_64& UUID)
    {
        uint64_t value = 0;
        stream >> value;
        UUID = UUID_64(value);
    }

}// namespace Wl

WL_HASH_DEFINE(Wl::UUID_64, UUID, { return UUID.GetValue(); })
