#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/RenderSurface.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    struct VulkanContext;

    class VulkanRenderSurface : public RHIRenderSurface
    {
    public:
        static SharedPtr<VulkanRenderSurface> Create(VulkanContext& context, void* nativeWindow);

        virtual void* GetPlatformWindow() override
        {
            return m_windowHandle;
        }

        virtual void Create() = 0;

        virtual void Destroy() = 0;

        VkSurfaceKHR& GetHandle()
        {
            return m_handle;
        }

    protected:
        VulkanRenderSurface(VkInstance instance, VkAllocationCallbacks* allocator, void* window_handle)
            : m_instance(instance)
            , m_allocator(allocator)
            , m_windowHandle(window_handle)
            , m_handle(VK_NULL_HANDLE)
        {
        }

    protected:
        VkInstance m_instance;
        VkSurfaceKHR m_handle;
        VkAllocationCallbacks* m_allocator;
        void* m_windowHandle;
    };

}// namespace Wl