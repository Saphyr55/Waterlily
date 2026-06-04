#include "Waterlily/Assets/assetsModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(AssetsModule, "Waterlily.Assets");

    void AssetsModule::OnLoad()
    {
        WL_LOG_INFO("[AssetsModule]", "Assets module loaded.");
    }

    void AssetsModule::OnStartup()
    {
        WL_LOG_INFO("[AssetsModule]", "Assets module started.");
    }

    void AssetsModule::OnShutdown()
    {
        WL_LOG_INFO("[AssetsModule]", "Assets module stopped.");
    }

    void AssetsModule::OnUnload()
    {
        WL_LOG_INFO("[AssetsModule]", "Assets module unloaded.");
    }

}// namespace Wl
