#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/IO/PlatformFileSystem.hpp"
#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    ModuleRegistry& ModuleRegistry::GetInstance()
    {
        static ModuleRegistry s_instance;
        return s_instance;
    }

    Module* ModuleRegistry::LoadModule(const StringRef name)
    {
        if (IsModuleLoaded(name))
        {
            return GetModule(name);
        }

        String filepathLib(name.data());
        filepathLib += DynamicLibraryLoader::Extension();

        SharedPtr<DynamicLibrary> library = nullptr;
        if (PlatformFileSystem::GetPlatform().FileExists(filepathLib))
        {
            library = DynamicLibraryLoader::Load(filepathLib.GetData());
        }
        else
        {
            LLOG_ERROR("[ModuleRegistry]", Wl::Format("Module \"%s\" not found.", name.data()));
            return nullptr;
        }

        if (!m_pendingModules.Contains(name))
        {
            LLOG_ERROR("[ModuleRegistry]", Wl::Format("Module \"%s\" is not registered and cannot be loaded.", name.data()))
            return nullptr;
        }

        const ModuleInitializerFunc& moduleInitializer = m_pendingModules.Get(name);
        Module* modulePtr = moduleInitializer();

        if (!modulePtr)
        {
            LLOG_ERROR("[ModuleRegistry]", Wl::Format("Module \"%s\" failed to initialize.", name.data()))
            return nullptr;
        }

        modulePtr->OnLoad();

        m_pendingModules.Remove(name);
        m_loadedModules.Put(name, LoadedModule(name, modulePtr, library));

        return modulePtr;
    }

    void ModuleRegistry::UnloadModule(const StringRef name)
    {
        LoadedModule* loadedModule = m_loadedModules.GetPtr(name);
        if (!loadedModule)
        {
            return;
        }

        if (!loadedModule->ModulePtr)
        {
            return;
        }

        loadedModule->ModulePtr->OnUnload();

        if (loadedModule->Library)
        {
            DynamicLibraryLoader::Unload(loadedModule->Library);
        }

        m_loadedModules.Remove(name);
    }

    bool ModuleRegistry::ExistsModule(const StringRef name) const
    {
        return m_pendingModules.Contains(name) || IsModuleLoaded(name);
    }

    bool ModuleRegistry::IsModuleLoaded(const StringRef name) const
    {
        return m_loadedModules.Contains(name);
    }

    Module* ModuleRegistry::GetModuleInterface(const StringRef name)
    {
        if (const LoadedModule* loadedModule = m_loadedModules.GetPtr(name))
        {
            return loadedModule->ModulePtr;
        }
        return nullptr;
    }

    void ModuleRegistry::RegisterModule(const StringRef name, const ModuleInitializerFunc& initializer)
    {
        if (!m_pendingModules.Contains(name))
        {
            m_pendingModules.Put(name, initializer);
            LLOG_DEBUG("[ModuleRegistry]", Wl::Format("The module '%s' has been registered.", name.data()));
        }
    }

    void ModuleRegistry::UnregisterModule(const StringRef name)
    {
        m_pendingModules.Remove(name);
    }

}// namespace Wl