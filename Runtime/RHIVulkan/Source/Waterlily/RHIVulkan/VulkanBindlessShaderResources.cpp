#include "VulkanBindlessShaderResources.hpp"
#include "Waterlily/Core/Algorithms/Algorithms.hpp"
#include "Waterlily/Core/Function/FunctionRef.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

namespace Wl
{

    VkDescriptorSetLayoutBinding VulkanBindlessShaderResources::BindingSetMapping(const RHIShaderResourceBinding& binding)
    {
        VkDescriptorType descriptorType = VulkanDescriptorTypeGet(binding.Type);
        VkShaderStageFlags stageFlags = VulkanShaderStageGet(binding.Stage);

        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = binding.Binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = m_maxResources;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;

        return layoutBinding;
    };

    void VulkanBindlessShaderResources::Create(uint32_t maxResources, const Array<RHIShaderResourceBinding>& bindings)
    {
        m_maxResources = maxResources;

        VulkanContext& context = VulkanContextGet();

        HashMap<VkDescriptorType, uint32_t> typeCounts(bindings.GetSize());
        for (const RHIShaderResourceBinding& binding: bindings)
        {
            typeCounts[VulkanDescriptorTypeGet(binding.Type)] = maxResources;
        }

        Array<VkDescriptorPoolSize> poolSizes;
        for (const auto [type, count]: typeCounts)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = type;
            poolSize.descriptorCount = count;
            poolSizes.Append(poolSize);
        }

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        descriptorPoolCreateInfo.maxSets = 1;
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.GetData();
        descriptorPoolCreateInfo.poolSizeCount = poolSizes.GetSize();

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateDescriptorPool(context.Device, &descriptorPoolCreateInfo, context.Allocator, &m_pool));

        Array<VkDescriptorBindingFlags> bindingFlags;
        bindingFlags.Resize(bindings.GetSize(),
                            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);

        Array<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.Resize(bindings.GetSize());

        Wl::Transform(bindings.begin(),
                      bindings.end(),
                      layoutBindings.begin(),
                      Wl::Bind(this, &VulkanBindlessShaderResources::BindingSetMapping));

        VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo{
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
                nullptr};
        extendedInfo.bindingCount = layoutBindings.GetSize();
        extendedInfo.pBindingFlags = bindingFlags.GetData();

        VkDescriptorSetLayoutCreateInfo descriptorPoolLayoutCreateInfo = {};
        descriptorPoolLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorPoolLayoutCreateInfo.pNext = &extendedInfo;
        descriptorPoolLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        descriptorPoolLayoutCreateInfo.bindingCount = layoutBindings.GetSize();
        descriptorPoolLayoutCreateInfo.pBindings = layoutBindings.GetData();

        m_layout.SetBindings(bindings);

        WL_VULKAN_CHECK(VulkanAPI::vkCreateDescriptorSetLayout(context.Device,
                                                               &descriptorPoolLayoutCreateInfo,
                                                               context.Allocator,
                                                               &m_layout.GetHandle()));

        VkDescriptorSetAllocateInfo descriptorSetAllocationInfo = {};
        descriptorSetAllocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocationInfo.descriptorPool = m_pool;
        descriptorSetAllocationInfo.descriptorSetCount = 1;
        descriptorSetAllocationInfo.pSetLayouts = &m_layout.GetHandle();

        m_srg.GetIndexPool(0);
        m_srg.SetLayout(&m_layout);

        WL_VULKAN_CHECK(
                VulkanAPI::vkAllocateDescriptorSets(context.Device, &descriptorSetAllocationInfo, &m_srg.GetHandle()));
    }

    void VulkanBindlessShaderResources::Destroy()
    {
        if (m_pool == VK_NULL_HANDLE)
        {
            return;
        }

        m_layout.Destroy();
        VulkanContext& context = VulkanContextGet();
        VulkanAPI::vkDestroyDescriptorPool(context.Device, m_pool, context.Allocator);
        m_pool = VK_NULL_HANDLE;
    }

}// namespace Wl
