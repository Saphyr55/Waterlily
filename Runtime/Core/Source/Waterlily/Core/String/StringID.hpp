#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <shared_mutex>

namespace Wl
{

    class WL_CORE_API StringID
    {
    public:
        static void Register(uint64_t hash, StringRef str);
        static StringRef Resolve(const StringID& sid);
        static StringRef Resolve(uint64_t hash);

    public:
        inline uint64_t GetHash() const
        {
            return m_hash;
        }

        StringRef GetText() const
        {
#if WL_DEBUG
            return m_text;
#else
            return StringID::Resolve(m_hash);
#endif
        }

    public:
        StringID() = default;
        StringID(uint64_t hash, StringRef text);

        StringID(const char* text)
            : StringID(StringRef(text))
        {
        }

        explicit StringID(StringRef text)
            : StringID(Wl::fnv1a_cstr(text.GetData(), text.GetSize()), text)
        {
        }

        ~StringID() = default;

        StringID(const StringID& other) = default;
        StringID(StringID&& other)
        {
            m_hash = other.m_hash;
            m_text = Resolve(m_hash).GetData();
        }

        StringID& operator=(const StringID& other) = default;
        StringID& operator=(StringID&& other)
        {
            m_hash = other.m_hash;
            m_text = Resolve(m_hash).GetData();
            return *this;
        }

        inline bool operator==(const StringID& other) const
        {
            return m_hash == other.m_hash;
        }

        inline bool operator!=(const StringID& other) const
        {
            return m_hash != other.m_hash;
        }

    private:
        struct Registry
        {
            static Registry& GetInstance();

            HashMap<uint64_t, String> m_registry;
            std::shared_mutex m_mutex;
        };

        uint64_t m_hash = 0;
#if WL_DEBUG
        const char* m_text = "";
#endif
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
        sid = StringID(hash, str);
    }

}// namespace Wl

WL_HASH_DEFINE(Wl::StringID, sid, { return sid.GetHash(); })
