#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Engine/engineExports.hpp"

namespace Wl
{

    class WL_ENGINE_API Engine
    {
    public:
        static Engine& GetInstance();

        void Startup();
        void Shutdown();

        StringRef GetProjectDirectory();
        StringRef GetEngineDirectory();

        Array<const ModuleManifestInformation*>& GetOrderedModuleInformations();

        ModuleManifest& GetManifest();

    public:
        Engine();
        ~Engine() = default;

        Engine(Engine&&) = delete;
        Engine(const Engine&) = delete;

        Engine& operator=(const Engine&) = delete;
        Engine& operator=(Engine&&) = delete;

    private:
        Array<const ModuleManifestInformation*> m_orderedModuleInformations;
        ModuleManifest m_manifest;
        StringRef m_projectDirectory;
    };

}// namespace Wl