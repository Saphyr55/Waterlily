#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"

#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

namespace Wl
{

    constexpr static auto BindingSetMapping = [](const RHIShaderResourceBinding& binding)
    {
        VkDescriptorType descriptorType = VulkanDescriptorTypeGet(binding.Type);
        VkShaderStageFlags stageFlags = VulkanShaderStageGet(binding.Stage);

        VkDescriptorSetLayoutBinding layout = {};
        layout.binding = binding.Binding;
        layout.descriptorType = descriptorType;
        layout.descriptorCount = binding.Count;
        layout.stageFlags = stageFlags;
        layout.pImmutableSamplers = nullptr;

        return layout;
    };

    RHIShaderResourceType VulkanShaderResourceGroupLayout::GetResourceType(size_t binding) const
    {
        for (const RHIShaderResourceBinding& shaderResourceBinding: m_bindings)
        {
            if (shaderResourceBinding.Binding == binding)
            {
                return shaderResourceBinding.Type;
            }
        }
        WL_LOG_FATAL("[Vulkan]", "Binding index not found in shader resource group layout.");
        return RHIShaderResourceType(-1);
    }

    void VulkanShaderResourceGroupLayout::Create(const Array<RHIShaderResourceBinding>& bindings)
    {
        VulkanContext& context = VulkanContextGet();
        m_bindings = bindings;

        Array<VkDescriptorSetLayoutBinding> vulkanBindings;
        vulkanBindings.Resize(m_bindings.size());

        Wl::Transform(m_bindings.begin(), m_bindings.end(), vulkanBindings.begin(), BindingSetMapping);

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = static_cast<uint32_t>(vulkanBindings.size());
        createInfo.pBindings = vulkanBindings.data();

        WL_VULKAN_CHECK(VulkanAPI::vkCreateDescriptorSetLayout(context.Device,
                                                               &createInfo,
                                                               context.Allocator,
                                                               &m_handle));
    }

    void VulkanShaderResourceGroupLayout::Destroy()
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            VulkanContext& context = VulkanContextGet();
            VulkanAPI::vkDestroyDescriptorSetLayout(context.Device, m_handle, context.Allocator);
            m_handle = VK_NULL_HANDLE;
        }
    }

}// namespace Wl
