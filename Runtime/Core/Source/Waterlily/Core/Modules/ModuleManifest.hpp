#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class ModuleManifestKeyNames
    {
    public:
        inline static constexpr StringRef Name = "Name";
        inline static constexpr StringRef Version = "Version";
        inline static constexpr StringRef Deps = "Deps";
    };

    struct ModuleInformation
    {
        Array<String> Dependencies;
        String Name = "";
        String Version = "";
    };

    class WL_CORE_API ModuleManifest
    {
    public:
        bool LoadJSON(StringRef filepath);

    public:
        Array<ModuleInformation>& GetModules();

        const Array<ModuleInformation>& GetModules() const;

        size_t GetModuleCount() const
        {
            return m_manifestInformations.GetSize();
        }
        
        void Clear()
        {
            m_manifestInformations.Clear();
        }

    private:
        Array<ModuleInformation> m_manifestInformations;
    };

    WL_CORE_API bool ModuleManifestResolveDependencies(const ModuleManifest& manifest,
                                                       Array<const ModuleInformation*>& out_order);
    WL_CORE_API void ModuleManifestLog(const ModuleManifest& manifest);

}// namespace Wl