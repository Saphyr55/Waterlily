#include "Waterlily/RHIVulkan/RHIVulkanModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RHIVulkanModule, "Waterlily.RHI.Vulkan");

    void RHIVulkanModule::OnLoad()
    {
        WL_LOG_INFO("RHIVulkanModule", "Loading RHI Vulkan Module.");
    }

    void RHIVulkanModule::OnStartup()
    {
        WL_LOG_INFO("RHIVulkanModule", "Startup RHI Vulkan Module.");
    }

    void RHIVulkanModule::OnShutdown()
    {
        WL_LOG_INFO("RHIVulkanModule", "Shuting down RHI Vulkan Module...");
    }

    void RHIVulkanModule::OnUnload()
    {
        WL_LOG_INFO("RHIVulkanModule", "Unload RHI Vulkan Module.");
    }

}// namespace Wl