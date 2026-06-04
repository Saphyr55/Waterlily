#pragma once

#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Semaphore.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanFence : public RHIFence
    {
    public:
        const VkFence& GetHandle() const
        {
            return m_handle;
        }

        VkFence& GetHandle()
        {
            return m_handle;
        }

        virtual bool IsSignaled() const override
        {
            return m_signaled;
        }

        inline void SetSignaled(bool signaled)
        {
            m_signaled = signaled;
        }

    public:
        VulkanFence(bool signaled)
            : m_handle(VK_NULL_HANDLE)
            , m_signaled(signaled)
        {
        }
        VulkanFence(VkFence fence, bool signaled)
            : m_handle(fence)
            , m_signaled(signaled)
        {
        }

        VulkanFence() = default;
        ~VulkanFence() = default;

    private:
        bool m_signaled;
        VkFence m_handle;
    };

    class VulkanSemaphore : public RHISemaphore
    {
    public:
        VkSemaphore& GetHandle()
        {
            return m_handle;
        }

    public:
        VulkanSemaphore(VkSemaphore semaphore)
            : m_handle(semaphore)
        {
        }

        VulkanSemaphore() = default;
        ~VulkanSemaphore() = default;

    private:
        VkSemaphore m_handle;
    };

}// namespace Wl