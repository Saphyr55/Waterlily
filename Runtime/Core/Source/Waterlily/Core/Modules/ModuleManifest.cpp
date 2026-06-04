#include "Waterlily/Core/Modules/ModuleManifest.hpp"

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include <lua.hpp>

namespace Wl
{

    static ModuleManifestInformation ModuleManifestInformationLuaParse(lua_State* L, int32_t tableIndex)
    {
        ModuleManifestInformation description;

        lua_getfield(L, -1, ModuleManifestKeyNames::Name);
        if (lua_isstring(L, -1))
        {
            description.Name = lua_tostring(L, -1);
        }
        else
        {
            LLOG_ERROR("[ModuleManifest]", "Module is missing a valid 'name' field.");
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, ModuleManifestKeyNames::Version);
        if (lua_isstring(L, -1))
        {
            description.Version = lua_tostring(L, -1);
        }
        else
        {
            LLOG_ERROR("[ModuleManifest]",
                       Wl::Format("Module '%s' is missing a valid 'version' field.", description.Name.GetData()));
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, ModuleManifestKeyNames::Dependencies);
        if (lua_istable(L, -1))
        {
            size_t len = lua_rawlen(L, -1);
            for (size_t i = 1; i <= len; i++)
            {
                lua_rawgeti(L, -1, i);
                if (lua_isstring(L, -1))
                {
                    description.Dependencies.Append(lua_tostring(L, -1));
                }
                else
                {
                    LLOG_ERROR(
                            "[ModuleManifest]",
                            Wl::Format("Module '%s' has a non-string dependency at index %zu.", description.Name.GetData(), i));
                }
                lua_pop(L, 1);
            }
        }
        else
        {
            LLOG_ERROR("[ModuleManifest]",
                       Wl::Format("Module '%s' ' has invalid 'dependencies', must be a table.", description.Name.GetData()))
        }

        lua_pop(L, 1);

        return description;
    }

    bool ModuleManifest::Merge(const ModuleManifest& other)
    {
        for (const ModuleManifestInformation& info: other.m_manifestInformations)
        {
            bool found = false;
            for (const ModuleManifestInformation& existingInfo: m_manifestInformations)
            {
                if (existingInfo.Name == info.Name)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                m_manifestInformations.Append(info);
            }
        }
        return true;
    }

    bool ModuleManifest::LoadLua(StringRef filepath)
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        if (luaL_dofile(L, filepath.data()) != LUA_OK)
        {
            lua_close(L);
            LLOG_ERROR("[ModuleManifest]", Wl::Format("Failed to load Lua file: ", filepath));
            return false;
        }

        if (!lua_istable(L, -1))
        {
            lua_close(L);
            LLOG_ERROR("[ModuleManifest]", "Lua manifest must return a table.");
            return false;
        }

        lua_getfield(L, -1, ManifestKeyNames::Modules);
        if (lua_istable(L, -1))
        {
            int32_t tableIndex = lua_gettop(L);
            lua_pushnil(L);

            while (lua_next(L, tableIndex) != 0)
            {
                if (lua_istable(L, -1))
                {
                    m_manifestInformations.Append(ModuleManifestInformationLuaParse(L, tableIndex));
                }
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
        }
        else
        {
            LLOG_ERROR("[ModuleManifest]", Wl::Format("Missing modules field in '%s' file.", filepath));
        }

        lua_pop(L, 1);
        lua_close(L);

        return true;
    }

    void ModuleManifestLog(const ModuleManifest& manifest)
    {
        const Array<ModuleManifestInformation>& moduleInformations = manifest.GetManifestInformations();
        LLOG_INFO("[ModuleManifest]", Wl::Format("Module Manifest Informations (%d modules):", moduleInformations.GetSize()));

        for (size_t i = 0; i < manifest.GetManifestInformations().GetSize(); i++)
        {
            const ModuleManifestInformation& moduleInformation = moduleInformations[i];
            LLOG_INFO("[ModuleManifest]", Wl::Format("Module:"));
            LLOG_INFO("[ModuleManifest]",
                      Wl::Format("  %s: %s", ModuleManifestKeyNames::Name, moduleInformation.Name.GetData()));
            LLOG_INFO("[ModuleManifest]",
                      Wl::Format("  %s: %s", ModuleManifestKeyNames::Version, moduleInformation.Version.GetData()));

            if (moduleInformation.Dependencies.IsEmpty())
            {
                LLOG_INFO("[ModuleManifest]", Wl::Format("  %s: []", ModuleManifestKeyNames::Dependencies));
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

            LLOG_INFO("[ModuleManifest]",
                      Wl::Format("  %s: [%s]", ModuleManifestKeyNames::Dependencies, dependencyNames.GetData()));
        }
    }

    Array<ModuleManifestInformation>& ModuleManifest::GetManifestInformations()
    {
        return m_manifestInformations;
    }

    const Array<ModuleManifestInformation>& ModuleManifest::GetManifestInformations() const
    {
        return m_manifestInformations;
    }

    bool ModuleManifestResolveDependencies(const ModuleManifest& manifest,
                                           Array<const ModuleManifestInformation*>& outOrder)
    {
        const size_t size = manifest.GetManifestInformations().GetSize();
        outOrder.Reserve(size);

        HashMap<StringRef, const ModuleManifestInformation*> moduleMap(size);
        HashMap<StringRef, size_t> indegree(size);
        HashMap<StringRef, Array<String>> graph(size);

        for (const ModuleManifestInformation& info: manifest.GetManifestInformations())
        {
            moduleMap[info.Name] = &info;
            indegree[info.Name] = 0;
            graph[info.Name] = Array<String>(size);
        }

        for (const ModuleManifestInformation& info: manifest.GetManifestInformations())
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
                    LLOG_ERROR("[ModuleManifest]",
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