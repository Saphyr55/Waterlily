#include "Waterlily/Launcher/Launcher.hpp"

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Platform.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/Engine/Application.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include "Waterlily/Engine/ProjectSettings.hpp"
#include "Waterlily/Launcher/CommandParser.hpp"

namespace Wl
{

    bool MainLoadManifest()
    {
        Engine& engine = Engine::GetInstance();
        ModuleManifest& engineManifest = engine.GetManifest();

        StringRef engineDirectory = engine.GetEngineDirectory();
        StringRef projectDirectory = engine.GetProjectDirectory();

        ModuleManifest applicationManifest;
        String applicationManifestFilepath = Wl::Format("%s/manifest.lua", projectDirectory.data());
        String engineManifestFilepath = Wl::Format("%s/manifest.lua", engineDirectory.data());

        if (!applicationManifest.LoadLua(applicationManifestFilepath))
        {
            LLOG_ERROR("[Main]", Wl::Format("Cannot load the manifest '%s'", applicationManifestFilepath.GetData()));
            return false;
        }

        if (!engine.GetManifest().LoadLua(engineManifestFilepath))
        {
            LLOG_ERROR("[Main]", Wl::Format("Cannot load the manifest '%s'", engineManifestFilepath.GetData()))
            return false;
        }

        engineManifest.Merge(applicationManifest);

        ModuleManifestLog(engineManifest);

        Array<const ModuleManifestInformation*>& order = engine.GetOrderedModuleInformations();

        if (!ModuleManifestResolveDependencies(engineManifest, order))
        {
            LLOG_ERROR("[Main]", "Cannot resolve manifest dependencies.");
            return false;
        }

        for (const ModuleManifestInformation* info: engine.GetOrderedModuleInformations())
        {
            if (!ModuleRegistry::GetInstance().LoadModule(info->Name))
            {
                LLOG_ERROR("[Main]", Wl::Format("Cannot load module: %s", info->Name.GetData()));
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
            const ModuleManifestInformation* info = engine.GetOrderedModuleInformations()[i];
            ModuleRegistry::GetInstance().UnloadModule(info->Name);
            LLOG_INFO("[Main]", Wl::Format("Unloaded module: %s", info->Name.GetData()));
        }
    }

    bool MainPreLaunch(int32_t argc, const char** argv)
    {
        LLOG_INFO("[Main]", "Pre init Engine.");

        HashMap<StringRef, StringRef> commands = CommandLineParse(argc, argv);
        StringRef projectdir = commands["projectdir"];
        StringRef enginedir = commands["enginedir"];

        ProjectSettings& settings = ProjectSettings::GetInstance();
        settings.Put("projectdir", projectdir);
        settings.Put("enginedir", enginedir);

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