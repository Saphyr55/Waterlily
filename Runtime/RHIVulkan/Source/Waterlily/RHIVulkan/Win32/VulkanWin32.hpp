#pragma once

#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"

namespace Wl
{

    class RHIVulkanWindowsSurface : public VulkanRenderSurface
    {
    public:
        virtual void Create() override;
        virtual void Destroy() override;

    public:
        RHIVulkanWindowsSurface(VulkanContext& context, void* windowHandle);
    };

}// namespace Wl
