#include "Waterlily/RHIVulkan/win32/VulkanWin32.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

namespace Wl
{

    SharedPtr<VulkanRenderSurface> VulkanRenderSurface::Create(VulkanContext& context, void* windowHandle)
    {
        return MakeShared<RHIVulkanWindowsSurface>(context, windowHandle);
    }

    RHIVulkanWindowsSurface::RHIVulkanWindowsSurface(VulkanContext& context, void* windowHandle)
        : VulkanRenderSurface(context.Instance, context.Allocator, windowHandle)
    {
    }

    void RHIVulkanWindowsSurface::Create()
    {
        WL_LOG_INFO("Vulkan", "Initializing Vulkan surface...");

        PFN_vkCreateWin32SurfaceKHR WL_vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
                VulkanAPI::vkGetInstanceProcAddr(m_instance, "vkCreateWin32SurfaceKHR"));

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
        surfaceCreateInfo.hwnd = (HWND)m_windowHandle;
        surfaceCreateInfo.flags = 0;

        WL_VULKAN_CHECK(WL_vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, m_allocator, &m_handle));
    }

    void RHIVulkanWindowsSurface::Destroy()
    {
        WL_LOG_INFO("Vulkan", "Destroying Vulkan surface...");

        if (m_handle != VK_NULL_HANDLE)
        {
            VulkanAPI::vkDestroySurfaceKHR(m_instance, m_handle, m_allocator);
            m_handle = VK_NULL_HANDLE;
            WL_LOG_INFO("Vulkan", "Vulkan surface destroyed.");
        }
    }

}// namespace Wl

#endif
