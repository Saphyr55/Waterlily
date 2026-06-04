#include "Waterlily/Scene/SceneModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(SceneModule, "Waterlily.Scene");

    void SceneModule::OnLoad()
    {
        LLOG_INFO("[SceneModule]", "Scene module loaded.");
    }

    void SceneModule::OnStartup()
    {
        LLOG_INFO("[SceneModule]", "Scene module started.");
    }

    void SceneModule::OnShutdown()
    {
        LLOG_INFO("[SceneModule]", "Scene module stopped.");
    }

    void SceneModule::OnUnload()
    {
        LLOG_INFO("[SceneModule]", "Scene module unloaded.");
    }

}// namespace Wl