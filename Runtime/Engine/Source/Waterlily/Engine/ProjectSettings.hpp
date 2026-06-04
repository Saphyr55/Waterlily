#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Engine/engineExports.hpp"

namespace Wl
{

    class WL_ENGINE_API ProjectSettings
    {
    public:
        static ProjectSettings& GetInstance();

        StringRef Get(StringRef key);
        void Put(StringRef key, StringRef value);

    private:
        HashMap<StringRef, String> m_settings;
    };

}// namespace Wl