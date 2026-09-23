#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <shared_mutex>

namespace Wl
{

    class WL_CORE_API StringID
    {
        struct Registry
        {
            static Registry& GetInstance();

            HashMap<uint64_t, String> m_registry;
            mutable std::shared_mutex m_mutex;
        };

    public:
        static void Register(uint64_t hash, StringRef str);
        static StringRef Resolve(const StringID& sid);
        static StringRef Resolve(uint64_t hash);

    public:
        StringRef GetText() const;
        uint64_t GetHash() const;

        bool operator==(const StringID& other) const;
        bool operator!=(const StringID& other) const;

    public:
        StringID() = default;
        StringID(uint64_t hash, StringRef text);
        explicit StringID(StringRef text);
        ~StringID() = default;

    private:
        uint64_t m_hash = 0;
#if WL_DEBUG
        StringRef m_text = "";
#endif
    };

    WL_CORE_API void operator<<(OutputStream& stream, const StringID& sid);
    WL_CORE_API void operator>>(InputStream& stream, StringID& sid);

}// namespace Wl

WL_HASH_DEFINE(Wl::StringID, sid, { return sid.GetHash(); })
