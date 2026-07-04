#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"

namespace Wl
{

    Engine::Engine()
        : m_delegate(nullptr)
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

    Application* Engine::GetApplication()
    {
        return m_application;
    }

    void Engine::SetApplication(Application* application)
    {
        m_application = application;
    }
    
    ApplicationDelegate* Engine::GetApplicationDelegate()
    {
        return m_delegate;
    }

    void Engine::SetApplicationDelegate(ApplicationDelegate* delegate)
    {
        m_delegate = delegate;
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