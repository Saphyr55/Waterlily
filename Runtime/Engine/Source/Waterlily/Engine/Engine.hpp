#pragma once

#include "EngineService.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Engine/EngineExports.hpp"
#include "Waterlily/Engine/EngineService.hpp"
#include "Waterlily/Engine/EngineUpdater.hpp"
#include <type_traits>

namespace Wl
{

    class WL_ENGINE_API Engine
    {
    public:
        static Engine& GetInstance();

        void RequestStop();

        void Startup();
        
        void Shutdown();

        void Run();

        inline bool IsRunning()
        {
            return m_isRunning;
        }

        inline bool IsPaused()
        {
            return m_paused;
        }

        inline void Pause()
        {
            m_paused = true;
        }

        inline void Unpause()
        {
            m_paused = false;
        }

        void RegisterUpdater(StringID name, const SharedPtr<EngineUpdater>& updater);

        void UnregisterUpdater(StringID name);

        void RegisterService(StringID name, const SharedPtr<EngineService>& updater);

        void UnregisterService(StringID name);

        SharedPtr<EngineService> GetService(StringID name);

        template<typename EngineServiceType>
            requires std::is_base_of_v<EngineService, EngineServiceType>
        SharedPtr<EngineServiceType> GetService(StringID name)
        {
            return StaticPtrCast<EngineServiceType>(GetService(name));
        }

        Array<const ModuleInformation*>& GetOrderedModuleInformations();
        ModuleManifest& GetManifest();

    public:
        Engine();
        ~Engine() = default;

        Engine(Engine&&) = delete;
        Engine(const Engine&) = delete;

        Engine& operator=(const Engine&) = delete;
        Engine& operator=(Engine&&) = delete;

    private:
        HashMap<StringID, SharedPtr<EngineService>> m_services;
        HashMap<StringID, SharedPtr<EngineUpdater>> m_engineUpdaters;
        Array<const ModuleInformation*> m_orderedModuleInformations;
        ModuleManifest m_manifest;
        StringRef m_projectDirectory;
        bool m_isRunning = false;
        bool m_paused = false;
    };

}// namespace Wl
