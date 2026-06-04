#pragma once

#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanCommandQueue : public RHICommandQueue
    {
    public:
        virtual void Submit(const Array<RHICommandBuffer*>& command_buffers,
                            const Array<RHISemaphore*>& wait_semaphores,
                            const Array<RHISemaphore*>& signal_semaphores,
                            const RHIFence* fence) const override;

        virtual void Present(RHISwapchain* swapchain, RHISemaphore* semaphore) const override;

        virtual void WaitIdle() override;

        virtual RHIQueueType GetQueueType() const override
        {
            return m_type;
        }

        VkQueue& GetHandle()
        {
            return m_handle;
        }

        virtual bool IsPresentMode() const override;

        inline uint32_t GetQueueFamilyIndex()
        {
            return m_queueFamilyIndex;
        }

    public:
        VulkanCommandQueue(VulkanContext& context,
                           VkQueue queue,
                           RHIQueueType type,
                           uint32_t queueFamilyIndex,
                           bool isPresentModeSupported);

    private:
        VulkanContext& m_context;
        VkQueue m_handle;
        RHIQueueType m_type;
        uint32_t m_queueFamilyIndex;
        bool m_isPresentModeSupported;
    };

}// namespace Wl