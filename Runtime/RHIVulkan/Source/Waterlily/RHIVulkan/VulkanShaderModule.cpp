#include "Waterlily/RHIVulkan/VulkanShaderModule.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    VulkanShaderModule::VulkanShaderModule(const Array<uint8_t>& byteCode)
        : m_byteCode(byteCode)
        , handle_(VK_NULL_HANDLE)
    {
    }

    void VulkanShaderModule::Create()
    {
        VulkanContext& context = VulkanContextGet();
        if (handle_ != VK_NULL_HANDLE)
        {
            return;
        }

        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = m_byteCode.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_byteCode.data());

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateShaderModule(context.Device, &shaderModuleCreateInfo, context.Allocator, &handle_));
    }

    void VulkanShaderModule::Destroy()
    {
        VulkanContext& context = VulkanContextGet();
        VulkanAPI::vkDestroyShaderModule(context.Device, handle_, context.Allocator);
    }

    VkShaderModule VulkanShaderModule::GetHandle()
    {
        return handle_;
    }

}// namespace Wl