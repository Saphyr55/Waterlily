#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanFramebuffer : public RHIFramebuffer
    {
    public:
        inline virtual const RHIFramebufferDescription& GetDescription() const override
        {
            return m_description;
        }

        inline virtual RHIFramebufferDescription& GetDescription() override
        {
            return m_description;
        }

        VkFramebuffer& GetHandle();

        const VkFramebuffer& GetHandle() const;

        void Create();

        void Destroy();

    public:
        VulkanFramebuffer(VulkanContext& context, const RHIFramebufferDescription& description);
        virtual ~VulkanFramebuffer() override;

    private:
        RHIFramebufferDescription m_description;
        VkFramebuffer m_handle;
        VulkanContext& m_context;
    };

}// namespace Wl