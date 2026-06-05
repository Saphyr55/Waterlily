#include "Waterlily/RHIVulkan/RHIVulkanModule.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/Display.hpp"
#include "Waterlily/RHI/RHIModule.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/vulkanDevice.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    WL_REGISTER_MODULE(RHIVulkanModule, "Waterlily.RHI.Vulkan");

    RHIVulkanModule::RHIVulkanModule()
        : m_windowHandle(Display::InvalidWindowHandle)
    {
    }

    void RHIVulkanModule::OnLoad()
    {
        WL_LOG_INFO("[RHIVulkanModule]", "Loading RHI Vulkan Module.");
    }

    void RHIVulkanModule::OnStartup()
    {
        WL_LOG_INFO("[RHIVulkanModule]", "Startup RHI Vulkan Module.");

        ModuleRegistry& registry = ModuleRegistry::GetInstance();
        RHIModule* rhiModule = registry.GetModule<RHIModule>("Waterlily.RHI");

        VulkanContextCreate(VulkanContextGet(), rhiModule->GetNativeWindow());

        m_device = MakeShared<VulkanDevice>();
        m_device->Init();
        rhiModule->SetDevice(m_device);
    }

    void RHIVulkanModule::OnShutdown()
    {
        VulkanContext& context = VulkanContextGet();
        char* stats = nullptr;

        vmaBuildStatsString(context.VmaAllocator, &stats, VK_TRUE);

        std::cout << stats;

        vmaFreeStatsString(context.VmaAllocator, stats);

        vmaDestroyAllocator(context.VmaAllocator);

        m_device->Shutdown();
        VulkanContextDestroy(context);

        WL_LOG_INFO("[RHIVulkanModule]", "Shuting down RHI Vulkan Module...");
    }

    void RHIVulkanModule::OnUnload()
    {
        WL_LOG_INFO("[RHIVulkanModule]", "Unload RHI Vulkan Module.");
    }

}// namespace Wl