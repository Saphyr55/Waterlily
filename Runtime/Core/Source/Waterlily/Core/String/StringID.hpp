#pragma once

#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/String/stringRegistry.hpp"
#include "Waterlily/Core/hash/fnv-1a.hpp"

namespace Wl
{

    class StringID
    {
    public:
        inline uint64_t GetHash() const
        {
            return m_hash;
        }

        constexpr StringID()
            : m_hash(0)
        {
        }

        constexpr StringID(uint64_t hash)
            : m_hash(hash)
        {
        }

        constexpr StringID(uint64_t hash, StringRef text)
            : m_hash(hash)
            , m_text(text)
        {
        }

        constexpr StringRef GetText() const
        {
            return m_text;
        }

        constexpr bool operator==(const StringID& other) const
        {
            return m_hash == other.m_hash;
        }

        constexpr bool operator!=(const StringID& other) const
        {
            return m_hash != other.m_hash;
        }

    private:
        uint64_t m_hash;
        StringRef m_text;
    };

    inline void operator<<(OutputStream& stream, const StringID& sid)
    {
        stream << sid.GetHash();
        stream << sid.GetText();
    }

    inline void operator>>(InputStream& stream, StringID& sid)
    {
        uint64_t hash = 0;
        String str;
        stream >> hash;
        stream >> str;
        sid = StringRegistry::GetInstance().RegisterSID(StringID(hash, str), str);
    }

    template<size_t Length>
        requires(Length > 0)
    consteval StringID InternalSID(const char (&str)[Length])
    {
        return StringID(Wl::fnv1a_cstr(str, Length - 1));
    }

    inline StringID CreateSID(const char* str, size_t length)
    {
        WL_CHECK(length > 0);
        StringID sid(Wl::fnv1a_cstr(str, length - 1));
        return StringRegistry::GetInstance().RegisterSID(sid, str); 
    }

    inline StringID CreateSID(StringRef str)
    {
        StringID sid(Wl::fnv1a_cstr(str.GetData(), str.GetSize()));
        return StringRegistry::GetInstance().RegisterSID(sid, str);
    }

}// namespace Wl

WL_HASH_DEFINE(Wl::StringID, sid, { return sid.GetHash(); })

#define WL_SID(str)                                                       \
    ([]() -> ::Wl::StringID {                                             \
        constexpr ::Wl::StringID sid = ::Wl::InternalSID(str);            \
        return ::Wl::StringRegistry::GetInstance().RegisterSID(sid, str); \
    }())
