#include "Waterlily/Engine/ProjectSettings.hpp"

namespace Wl
{

    ProjectSettings& ProjectSettings::GetInstance()
    {
        static ProjectSettings s_settings;
        return s_settings;
    }

    StringRef ProjectSettings::Get(StringRef name)
    {
        return m_settings[name];
    }

    void ProjectSettings::Put(StringRef name, StringRef value)
    {
        m_settings[name] = String(value);
    }

}// namespace Wl