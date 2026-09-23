#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"

#include <shared_mutex>

namespace Wl
{

    StringID::Registry& StringID::Registry::GetInstance()
    {
        static Registry s_registry;
        return s_registry;
    }

    void StringID::Register(uint64_t hash, StringRef str)
    {
        Registry& registry = Registry::GetInstance();
        std::unique_lock lock(registry.m_mutex);
        if (!registry.m_registry.Contains(hash))
        {
            registry.m_registry.Put(hash, String(str));
        }
    }

    StringRef StringID::Resolve(uint64_t hash)
    {
        Registry& registry = Registry::GetInstance();

        std::shared_lock lock(registry.m_mutex);

        auto it = registry.m_registry.find(hash);
        if (it != registry.m_registry.end())
        {
            return it->Value.GetData();
        }

        return nullptr;
    }

    StringRef StringID::Resolve(const StringID& sid)
    {
        return Resolve(sid.GetHash());
    }

    StringID::StringID(uint64_t hash, StringRef text)
        : m_hash(hash)
#if WL_DEBUG
        , m_text(text)
#endif
    {
        StringID::Register(m_hash, text);
    }

    StringID::StringID(StringRef text)
        : m_hash(Wl::fnv1a_cstr(text.GetData(), text.GetSize()))
#if WL_DEBUG
        , m_text(text)
#endif
    {
        StringID::Register(m_hash, text);
    }

    StringRef StringID::GetText() const
    {
#if WL_DEBUG
        return m_text;
#else
        return StringID::Resolve(m_hash);
#endif
    }

    uint64_t StringID::GetHash() const
    {
        return m_hash;
    }

    bool StringID::operator==(const StringID& other) const
    {
        return m_hash == other.m_hash;
    }

    bool StringID::operator!=(const StringID& other) const
    {
        return m_hash != other.m_hash;
    }

    void operator<<(OutputStream& stream, const StringID& sid)
    {
        stream << sid.GetHash();
        stream << sid.GetText();
    }

    void operator>>(InputStream& stream, StringID& sid)
    {
        uint64_t hash = 0;
        String str;
        stream >> hash;
        stream >> str;
        sid = StringID(hash, str);
    }

}// namespace Wl