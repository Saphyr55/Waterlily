#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Engine/ProjectSettings.hpp"

namespace Wl
{

    Engine::Engine()
        : m_orderedModuleInformations(64)
    {
    }

    Engine& Engine::GetInstance()
    {
        static Engine s_engine;
        return s_engine;
    }

    void Engine::Startup()
    {
        ModuleRegistry& module_registry = ModuleRegistry::GetInstance();

        for (const ModuleInformation* info: GetOrderedModuleInformations())
        {
            Module* module = module_registry.GetModuleInterface(info->Name);
            module->OnStartup();
        }
    }

    void Engine::Shutdown()
    {
        ModuleRegistry& module_registry = ModuleRegistry::GetInstance();

        for (size_t i = m_orderedModuleInformations.GetSize() - 1; i-- > 0;)
        {
            const ModuleInformation* info = m_orderedModuleInformations[i];
            Module* module = module_registry.GetModuleInterface(info->Name);
            module->OnShutdown();
        }
    }

    Array<const ModuleInformation*>& Engine::GetOrderedModuleInformations()
    {
        return m_orderedModuleInformations;
    }

    ModuleManifest& Engine::GetManifest()
    {
        return m_manifest;
    }

}// namespace Wl