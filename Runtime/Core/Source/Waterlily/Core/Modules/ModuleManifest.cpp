#include "Waterlily/Core/Modules/ModuleManifest.hpp"

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace Wl
{

    bool ModuleManifest::LoadJSON(StringRef filepath)
    {
        std::ifstream stream(filepath.data());
        if (!stream.is_open())
        {
            WL_LOG_ERROR("[ModuleManifest]", Wl::Format(" Cannot open \"%s\"", filepath.data()));
            return false;
        }

        nlohmann::json json;

        try
        {
            stream >> json;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            WL_LOG_ERROR("[ModuleManifest]", Wl::Format("JSON parse error in \"%s\": \"%s\"", filepath.data(), e.what()));
            return false;
        }

        m_manifestInformations.Reserve(json.size());

        for (auto& entry: json)
        {
            if (!entry.is_object())
            {
                WL_LOG_WARN("[ModuleManifest]", "Skipping non-object entry in the module manifest");
                continue;
            }

            ModuleInformation info;
            info.Name = entry.value(ModuleManifestKeyNames::Name.data(), "").c_str();
            info.Version = entry.value(ModuleManifestKeyNames::Version.data(), "").c_str();

            if (const auto depsIt = entry.find(ModuleManifestKeyNames::Deps.data()); depsIt != entry.end() && depsIt->is_array())
            {
                info.Dependencies.Reserve(depsIt->size());
                for (const auto& dep: *depsIt)
                {
                    if (dep.is_string())
                    {
                        info.Dependencies.Append(dep.get<std::string>().c_str());
                    }
                }
            }

            m_manifestInformations.Append(info);
        }

        return true;
    }

    void ModuleManifestLog(const ModuleManifest& manifest)
    {
        const Array<ModuleInformation>& moduleInformations = manifest.GetModules();
        WL_LOG_INFO("[ModuleManifest]", Wl::Format("Module Manifest Informations (%d modules):", moduleInformations.GetSize()));

        for (size_t i = 0; i < manifest.GetModules().GetSize(); i++)
        {
            const ModuleInformation& moduleInformation = moduleInformations[i];
            WL_LOG_INFO("[ModuleManifest]", "Module:");
            WL_LOG_INFO("[ModuleManifest]",
                        Wl::Format("  %s: %s", ModuleManifestKeyNames::Name, moduleInformation.Name.GetData()));
            WL_LOG_INFO("[ModuleManifest]",
                        Wl::Format("  %s: %s", ModuleManifestKeyNames::Version, moduleInformation.Version.GetData()));

            if (moduleInformation.Dependencies.IsEmpty())
            {
                WL_LOG_INFO("[ModuleManifest]", Wl::Format("  %s: []", ModuleManifestKeyNames::Deps));
                continue;
            }

            const size_t dependencyCount = moduleInformation.Dependencies.GetSize();
            String dependencyNames(256);
            for (size_t j = 0; j < dependencyCount; j++)
            {
                dependencyNames.Append(moduleInformation.Dependencies[j]);
                if (j < dependencyCount - 1)
                {
                    dependencyNames.Append(", ");
                }
            }

            WL_LOG_INFO("[ModuleManifest]",
                        Wl::Format("  %s: [%s]", ModuleManifestKeyNames::Deps, dependencyNames.GetData()));
        }
    }

    Array<ModuleInformation>& ModuleManifest::GetModules()
    {
        return m_manifestInformations;
    }

    const Array<ModuleInformation>& ModuleManifest::GetModules() const
    {
        return m_manifestInformations;
    }

    bool ModuleManifestResolveDependencies(const ModuleManifest& manifest,
                                           Array<const ModuleInformation*>& outOrder)
    {
        const size_t size = manifest.GetModules().GetSize();
        outOrder.Reserve(size);

        HashMap<StringRef, const ModuleInformation*> moduleMap(size);
        HashMap<StringRef, size_t> indegree(size);
        HashMap<StringRef, Array<String>> graph(size);

        for (const ModuleInformation& info: manifest.GetModules())
        {
            moduleMap[info.Name] = &info;
            indegree[info.Name] = 0;
            graph[info.Name] = Array<String>(size);
        }

        for (const ModuleInformation& info: manifest.GetModules())
        {
            for (const StringRef& dependency: info.Dependencies)
            {
                if (Array<String>* successors = graph.GetPtr(dependency))
                {
                    successors->Append(info.Name);
                    indegree[info.Name]++;
                }
                else
                {
                    WL_LOG_ERROR("[ModuleManifest]",
                                 Wl::Format("Module '%s' has an unknown dependency '%s'.", info.Name.GetData(), dependency.data()));
                }
            }
        }

        Array<StringRef> pending(size);
        for (auto [name, dependency]: indegree)
        {
            if (dependency == 0)
            {
                pending.Append(name);
            }
        }

        while (!pending.IsEmpty())
        {
            StringRef current = pending.Back();
            pending.Pop();
            outOrder.Append(moduleMap[current]);

            for (const StringRef& next: graph[current])
            {
                if (--indegree[next] == 0)
                {
                    pending.Append(next);
                }
            }
        }

        return outOrder.GetSize() == size;
    }

}// namespace Wl