#include "Waterlily/RHIVulkan/RHIVulkanModule.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    WL_REGISTER_MODULE(RHIVulkanModule, "Waterlily.RHI.Vulkan");

    void RHIVulkanModule::OnStartup()
    {
        WL_LOG_INFO("RHIVulkanModule", "RHI Vulkan Module started.");
    }

    void RHIVulkanModule::OnShutdown()
    {
        WL_LOG_INFO("RHIVulkanModule", "RHI Vulkan Module stopped.");
    }

}// namespace Wl