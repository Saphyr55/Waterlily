#include "Waterlily/Scene/SceneModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(SceneModule, "Waterlily.Scene");

    void SceneModule::OnLoad()
    {
        WL_LOG_INFO("[SceneModule]", "Scene module loaded.");
    }

    void SceneModule::OnStartup()
    {
        WL_LOG_INFO("[SceneModule]", "Scene module started.");
    }

    void SceneModule::OnShutdown()
    {
        WL_LOG_INFO("[SceneModule]", "Scene module stopped.");
    }

    void SceneModule::OnUnload()
    {
        WL_LOG_INFO("[SceneModule]", "Scene module unloaded.");
    }

}// namespace Wl