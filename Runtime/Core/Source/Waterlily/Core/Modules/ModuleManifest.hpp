#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    struct ModuleManifestInformation
    {
        Array<String> Dependencies;
        String Name = "";
        String Version = "";
    };

    class ManifestKeyNames
    {
    public:
        inline static constexpr StringRef Modules = "modules";
    };

    class ModuleManifestKeyNames
    {
    public:
        inline static constexpr StringRef Name = "name";
        inline static constexpr StringRef Version = "version";
        inline static constexpr StringRef Dependencies = "dependencies";
    };

    class WL_CORE_API ModuleManifest
    {
    public:
        bool LoadLua(StringRef filepath);

        bool Merge(const ModuleManifest& other);

    public:
        Array<ModuleManifestInformation>& GetManifestInformations();
        const Array<ModuleManifestInformation>& GetManifestInformations() const;

    private:
        Array<ModuleManifestInformation> m_manifestInformations;
    };

    WL_CORE_API bool ModuleManifestResolveDependencies(const ModuleManifest& manifest,
                                                       Array<const ModuleManifestInformation*>& out_order);
    WL_CORE_API void ModuleManifestLog(const ModuleManifest& manifest);

}// namespace Wl