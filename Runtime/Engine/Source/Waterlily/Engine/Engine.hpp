#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Engine/EngineExports.hpp"

namespace Wl
{
    class Application;
    class ApplicationDelegate;

    class WL_ENGINE_API Engine
    {
    public:
        static Engine& GetInstance();

        void Startup();
        void Shutdown();

        Application* GetApplication();
        void SetApplication(Application* application);

        ApplicationDelegate* GetApplicationDelegate();
        void SetApplicationDelegate(ApplicationDelegate* delegate);

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
        Array<const ModuleInformation*> m_orderedModuleInformations;
        ModuleManifest m_manifest;
        StringRef m_projectDirectory;
        Application* m_application;
        ApplicationDelegate* m_delegate;
    };

}// namespace Wl
