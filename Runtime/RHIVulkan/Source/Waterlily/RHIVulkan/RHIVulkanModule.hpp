#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/RHIVulkan/RHIVulkanExports.hpp"

namespace Wl
{

    class WL_RHI_VULKAN_API RHIVulkanModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;
    };

}// namespace Wl