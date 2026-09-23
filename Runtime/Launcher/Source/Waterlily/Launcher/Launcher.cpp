#include "Waterlily/Launcher/Launcher.hpp"

#include "Waterlily/Core/Logging/ConsoleLoggerWriter.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleManifest.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Platform.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Engine/Engine.hpp"
#include <cmath>
#include <cstdint>
#include <exception>

namespace Wl
{

    bool MainLoadManifest(StringRef manifestFilepath)
    {
        Engine& engine = Engine::GetInstance();
        ModuleManifest& engineManifest = engine.GetManifest();

        if (!engineManifest.LoadJSON(manifestFilepath))
        {
            WL_LOG_ERROR("Launcher", "Cannot load the manifest '%s'", manifestFilepath.data());
            return false;
        }

        ModuleManifestLog(engineManifest);

        Array<const ModuleInformation*>& order = engine.GetOrderedModuleInformations();

        if (!ModuleManifestResolveDependencies(engineManifest, order))
        {
            WL_LOG_ERROR("Launcher", "Cannot resolve manifest dependencies.");
            return false;
        }

        for (const ModuleInformation* info: engine.GetOrderedModuleInformations())
        {
            if (!ModuleRegistry::GetInstance().LoadModule(info->Name))
            {
                WL_LOG_ERROR("Launcher", "Cannot load module: %s", info->Name.GetData());
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
        }
    }

    bool MainPreLaunch(int32_t argc, const char** argv)
    {
        Logger::RegisterWriter(ConsoleLoggerWriter::Name, MakeShared<ConsoleLoggerWriter>());

        PlatformStartup();

        if (!MainLoadManifest("ModuleManifest.json"))
        {
            return false;
        }

        return true;
    }

    void MainPostLaunch()
    {
        MainUnloadManifest();
        PlatformShutdown();
    }

    int32_t MainConsole(int32_t argc, const char* argv[], MainConsoleCallback* callback)
    {
        int32_t result = EXIT_FAILURE;

        if (!MainPreLaunch(argc, argv))
        {
            return result;
        }

        try
        {
            Engine::GetInstance().StartupModules();
            result = callback();
            Engine::GetInstance().ShutdownModules();
        }
        catch (std::exception e)
        {
            WL_LOG_ERROR("Launcher", "%s", e.what());
        }

        MainPostLaunch();

        return result;
    }

    int32_t MainApplication(int32_t argc, const char* argv[])
    {
        int32_t result = EXIT_FAILURE;

        if (!MainPreLaunch(argc, argv))
        {
            return result;
        }
        
        try
        {
            Engine::GetInstance().StartupModules();

            Engine::GetInstance().Startup();
            Engine::GetInstance().Run();
            Engine::GetInstance().Shutdown();

            Engine::GetInstance().ShutdownModules();

            result = EXIT_SUCCESS;
        }
        catch (std::exception e)
        {
            WL_LOG_ERROR("Launcher", "%s", e.what());
        }
        
        MainPostLaunch();

        return result;
    }

}// namespace Wl