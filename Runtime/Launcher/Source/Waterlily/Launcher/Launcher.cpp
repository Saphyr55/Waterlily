#include "Waterlily/Launcher/Launcher.hpp"

#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Platform.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/Engine/Engine.hpp"

namespace Wl
{

    bool MainLoadManifest()
    {
        Engine& engine = Engine::GetInstance();
        ModuleManifest& engineManifest = engine.GetManifest();

        StringRef manifestFilepath = "ModuleManifest.json";

        if (!engineManifest.LoadJSON(manifestFilepath))
        {
            WL_LOG_ERROR("[Launcher]", Wl::Format("Cannot load the manifest '%s'", manifestFilepath.data()))
            return false;
        }

        ModuleManifestLog(engineManifest);

        Array<const ModuleInformation*>& order = engine.GetOrderedModuleInformations();

        if (!ModuleManifestResolveDependencies(engineManifest, order))
        {
            WL_LOG_ERROR("[Launcher]", "Cannot resolve manifest dependencies.");
            return false;
        }

        for (const ModuleInformation* info: engine.GetOrderedModuleInformations())
        {
            if (!ModuleRegistry::GetInstance().LoadModule(info->Name))
            {
                WL_LOG_ERROR("[Launcher]", Wl::Format("Cannot load module: %s", info->Name.GetData()));
                return false;
            }
        }

        return true;
    }

    void MainUnloadManifest()
    {
        Engine& engine = Engine::GetInstance();
        for (int32_t i = engine.GetOrderedModuleInformations().GetSize() - 1; i >= 0; i--)
        {
            const ModuleInformation* info = engine.GetOrderedModuleInformations()[i];
            ModuleRegistry::GetInstance().UnloadModule(info->Name);
            WL_LOG_INFO("[Launcher]", Wl::Format("Unloaded module: %s", info->Name.GetData()));
        }
    }

    bool MainPreLaunch(int32_t argc, const char** argv)
    {
        WL_LOG_INFO("[Launcher]", "Pre init Engine.");

        if (!MainLoadManifest())
        {
            return false;
        }

        return true;
    }

    void MainPostLaunch()
    {
        MainUnloadManifest();
    }

    int32_t Main(int32_t argc, const char** argv, EngineAppCallback* func)
    {
        PlatformStartup();
        if (!MainPreLaunch(argc, argv))
        {
            return EXIT_FAILURE;
        }

        Engine& engine = Engine::GetInstance();
        engine.Startup();

        Application application;

        int32_t result = func(application);

        engine.Shutdown();

        MainPostLaunch();
        PlatformShutdown();

        return result;
    }

}// namespace Wl