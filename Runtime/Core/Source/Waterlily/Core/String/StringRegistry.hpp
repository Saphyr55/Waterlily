#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <shared_mutex>

namespace Wl
{

    class StringID;

    class WL_CORE_API StringRegistry
    {
    public:
        static StringRegistry& GetInstance();

        StringID RegisterSID(StringID sid, StringRef str);

        StringRef Resolve(StringID sid);

    public:
        StringRegistry() = default;
        ~StringRegistry() = default;

    private:
        HashMap<uint64_t, String> m_registry;
        mutable std::shared_mutex m_mutex;
    };

}// namespace Wl