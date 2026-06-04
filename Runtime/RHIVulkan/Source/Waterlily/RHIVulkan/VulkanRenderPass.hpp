#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanRenderPass : public RHIRenderPass
    {
    public:
        inline virtual const RHIRenderPassDescription& GetDescription() const override
        {
            return m_description;
        }

        VkRenderPass& GetHandle()
        {
            return m_renderPass;
        }

        const VkRenderPass& GetHandle() const
        {
            return m_renderPass;
        }

        void Create();

        void Destroy();

    public:
        VulkanRenderPass(VulkanContext& context, const RHIRenderPassDescription& description)
            : m_context(context)
            , m_description(description)
            , m_renderPass(VK_NULL_HANDLE)
        {
        }

    private:
        VulkanContext& m_context;
        RHIRenderPassDescription m_description;
        VkRenderPass m_renderPass;
    };

    using RHIVulkanRenderPassRef = SharedPtr<VulkanRenderPass>;

}// namespace Wl