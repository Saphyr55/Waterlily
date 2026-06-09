#pragma once

#include <concepts>
#include <functional>
#include <ostream>

#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/Asserts.hpp"

namespace Wl
{

    template<typename CharType>
    class StringRefBase
    {
    public:
        constexpr StringRefBase()
            : m_data("")
        {
        }

        constexpr StringRefBase(const CharType* data)
            : m_data(data)
        {
            WL_CHECK(data);
        }

        constexpr StringRefBase(const String& str)
            requires std::same_as<CharType, char>
            : m_data(str.GetData())
        {
            WL_CHECK(str.GetData());
        }

        constexpr ~StringRefBase() = default;

        constexpr StringRefBase(const StringRefBase& other) = default;

        constexpr StringRefBase& operator=(const StringRefBase& other) = default;

        constexpr const CharType& operator[](size_t index) const
        {
            WL_CHECK(index < GetSize());
            return m_data[index];
        }

        constexpr operator const CharType*() const
        {
            return m_data;
        }

        constexpr const CharType* GetData() const
        {
            return m_data;
        }

        constexpr size_t GetSize() const
        {
            return StringLength(m_data);
        }

        constexpr bool IsEmpty() const
        {
            return GetSize() == 0;
        }

        constexpr bool operator==(const StringRefBase& other) const
        {
            return GetSize() == other.GetSize() && StringCompare(m_data, other.m_data) == 0;
        }

        constexpr bool operator==(const CharType* str) const
        {
            return *this == StringRefBase(str);
        }

        constexpr const CharType* data() const
        {
            return GetData();
        }

    private:
        const CharType* m_data;
    };

    using StringRef = StringRefBase<char>;
    using String8Ref = StringRefBase<char8_t>;
    using String16Ref = StringRefBase<char16_t>;
    using String32Ref = StringRefBase<char32_t>;
    using WStringRef = StringRefBase<wchar_t>;

    template<typename CharType>
    inline void operator<<(OutputStream& stream, const StringRefBase<CharType>& str)
    {
        const CharType* data = str.GetData();
        const uint8_t* buffer = reinterpret_cast<const uint8_t*>(data);
        uint64_t size = static_cast<uint64_t>(str.GetSize());
        stream << size;
        stream.Write(buffer, str.GetSize() * sizeof(CharType));
    }

    template<typename CharType>
    std::ostream& operator<<(std::ostream& os, const Wl::StringRefBase<CharType>& str)
    {
        os << str.GetData();
        return os;
    }

    template<typename CharType>
    inline constexpr Wl::StringBase<CharType> operator+(Wl::StringRefBase<CharType> lhs, Wl::StringRefBase<CharType> rhs)
    {
        Wl::StringBase<CharType> base(lhs);
        base.Append(rhs);
        return std::move(base);
    }

    template<typename CharType>
    inline constexpr Wl::StringBase<CharType> operator+(Wl::StringRefBase<CharType> lhs, const CharType* rhs)
    {
        Wl::StringBase<CharType> base(lhs);
        base.Append(rhs);
        return std::move(base);
    }

    template<typename CharType>
    constexpr Wl::StringBase<CharType> operator+(const CharType* lhs, Wl::StringRefBase<CharType> rhs)
    {
        Wl::StringBase<CharType> base(lhs);
        base.Append(rhs);
        return std::move(base);
    }

}// namespace Wl

WL_HASH_DEFINE(Wl::StringRef, s, {
    return Wl::StringHash(s.GetData(), s.GetSize());
})

WL_HASH_DEFINE(Wl::WStringRef, s, {
    return Wl::StringHash(s.GetData(), s.GetSize());
})
