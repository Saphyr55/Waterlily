#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"
#include "Waterlily/RHIVulkan/RHIVulkanExports.hpp"
#include "Waterlily/RHIVulkan/vulkanDevice.hpp"

namespace Wl
{

    class WL_RHI_VULKAN_API RHIVulkanModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

    public:
        void SetWindowHandle(WindowHandle window_handle)
        {
            m_windowHandle = window_handle;
        }

    public:
        RHIVulkanModule();

    private:
        SharedPtr<VulkanDevice> m_device;
        WindowHandle m_windowHandle;
    };

}// namespace Wl