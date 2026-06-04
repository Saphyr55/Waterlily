#include "Waterlily/Core/String/stringRegistry.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    StringRegistry& StringRegistry::GetInstance()
    {
        static StringRegistry s_registry;
        return s_registry;
    }

    StringID StringRegistry::RegisterSID(StringID sid, StringRef str)
    {
        std::unique_lock lock(m_mutex);

        if (!m_registry.Contains(sid.GetHash()))
        {
            m_registry.Emplace(sid.GetHash(), String(str));
        }

        return {sid.GetHash(), m_registry[sid.GetHash()]};
    }

    StringRef StringRegistry::Resolve(StringID sid)
    {
        std::shared_lock lock(m_mutex);

        auto it = m_registry.find(sid.GetHash());
        if (it != m_registry.end())
        {
            return (*it).Value;
        }

        return nullptr;
    }

}// namespace Wl