#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/Core/Platform/PlatformTime.hpp"

namespace Wl
{

    Engine::Engine()
    {
    }

    Engine& Engine::GetInstance()
    {
        static Engine s_engine;
        return s_engine;
    }

    void Engine::Startup()
    {
        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();

        for (const ModuleInformation* info: GetOrderedModuleInformations())
        {
            Module* module = moduleRegistry.GetModuleInterface(info->Name);
            module->OnStartup();
        }

        for (auto [name, service]: m_services)
        {
            service->OnStartup();
        }

        for (auto [name, updater]: m_engineUpdaters)
        {
            updater->OnStartup();
        }
    }

    void Engine::Shutdown()
    {

        for (auto [name, updater]: m_engineUpdaters)
        {
            updater->OnShutdown();
        }

        for (auto [name, service]: m_services)
        {
            service->OnShutdown();
        }

        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();

        for (size_t i = m_orderedModuleInformations.GetSize() - 1; i-- > 0;)
        {
            const ModuleInformation* info = m_orderedModuleInformations[i];
            Module* module = moduleRegistry.GetModuleInterface(info->Name);
            module->OnShutdown();
        }
    }

    void Engine::Run()
    {
        m_isRunning = true;
        double lastTime = PlatformGetHighResolutionTime();

        while (IsRunning())
        {
            Display::GetDefault().HandleEvents();

            double nowTime = PlatformGetHighResolutionTime();
            double deltaTime = nowTime - lastTime;
            lastTime = nowTime;

            if (!IsPaused())
            {
                for (auto [name, updater]: m_engineUpdaters)
                {
                    updater->OnTick(deltaTime);
                }
            }
        }
    }

    void Engine::RequestStop()
    {
        m_isRunning = false;
    }

    void Engine::RegisterUpdater(StringID name, const SharedPtr<EngineUpdater>& updater)
    {
        m_engineUpdaters[name] = updater;
    }

    void Engine::UnregisterUpdater(StringID name)
    {
        m_engineUpdaters.Remove(name);
    }

    void Engine::RegisterService(StringID name, const SharedPtr<EngineService>& service)
    {
        m_services[name] = service;
    }

    void Engine::UnregisterService(StringID name)
    {
        m_services.Remove(name);
    }
    
    SharedPtr<EngineService> Engine::GetService(StringID name)
    {
        return m_services[name];
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