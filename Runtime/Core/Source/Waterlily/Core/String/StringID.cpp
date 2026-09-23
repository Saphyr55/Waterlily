#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"

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

        if (auto* text = registry.m_registry.GetPtr(hash))
        {
            return *text;
        }

        return nullptr;
    }

    StringRef StringID::Resolve(const StringID& sid)
    {
        return Resolve(sid.GetHash());
    }

    StringID::StringID(uint64_t hash, StringRef text)
        : m_hash(hash)
    {
        StringID::Register(m_hash, text);
#if WL_DEBUG
        m_text = Resolve(m_hash);
#endif
    }

}// namespace Wl