#include "Waterlily/RHIVulkan/VulkanDescriptorSetPool.hpp"

#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"

namespace Wl
{

    RHIShaderResourceGroup* VulkanShaderResourceGroupPool::AllocateSRG(RHIShaderResourceGroupLayout* layout)
    {
        WL_CHECK(layout);

        VulkanContext& context = VulkanContextGet();

        VulkanShaderResourceGroupLayout* vulkanLayout = static_cast<VulkanShaderResourceGroupLayout*>(layout);
        VkDescriptorSetLayout vulkanLayoutHandle = vulkanLayout->GetHandle();

        VkDescriptorSetAllocateInfo allocationInfo = {};
        allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocationInfo.descriptorPool = m_handle;
        allocationInfo.descriptorSetCount = 1;
        allocationInfo.pSetLayouts = &vulkanLayoutHandle;

        size_t groupIndex;
        if (!m_freeGroups.IsEmpty())
        {
            groupIndex = *m_freeGroups.begin();
            m_freeGroups.Remove(groupIndex);
        }
        else
        {
            WL_CHECK(m_nextIndexToAllocate < m_maxGroupsCount);
            groupIndex = m_nextIndexToAllocate++;
        }

        VulkanShaderResourceGroup* newGroup = &m_allocatedGroups[groupIndex];
        newGroup->Init(groupIndex, layout);

        WL_VULKAN_CHECK(
                VulkanAPI::vkAllocateDescriptorSets(context.Device, &allocationInfo, &newGroup->GetHandle()));

        return newGroup;
    }

    RHIShaderResourceGroup* VulkanShaderResourceGroupPool::GetSRG(size_t poolIndex)
    {
        VulkanShaderResourceGroup& group = m_allocatedGroups[poolIndex];
        if (group.GetHandle() == VK_NULL_HANDLE)
        {
            return nullptr;
        }
        return &group;
    }

    void VulkanShaderResourceGroupPool::DeallocateSRG(RHIShaderResourceGroup* group)
    {
        WL_CHECK(group);

        VulkanContext& context = VulkanContextGet();
        VulkanShaderResourceGroup* vulkanSRG = static_cast<VulkanShaderResourceGroup*>(group);
        VkDescriptorSet& descriptorSet = vulkanSRG->GetHandle();

        m_freeGroups.Add(vulkanSRG->GetIndexPool());

        WL_VULKAN_CHECK(VulkanAPI::vkFreeDescriptorSets(context.Device, m_handle, 1, &descriptorSet));

        vulkanSRG->Reset();
    }

    void VulkanShaderResourceGroupPool::Reset()
    {
        m_nextIndexToAllocate = 0;
        VulkanAPI::vkResetDescriptorPool(VulkanContextGet().Device, m_handle, 0);
        m_freeGroups.Clear();
    }

    void VulkanShaderResourceGroupPool::Create(size_t maxGroupsCount, const Array<RHIShaderResourceBinding>& totalBindings)
    {
        m_maxGroupsCount = maxGroupsCount;

        m_nextIndexToAllocate = 0;
        m_allocatedGroups.Resize(maxGroupsCount);

        VulkanContext& context = VulkanContextGet();

        HashMap<VkDescriptorType, uint32_t> type_counts(totalBindings.GetSize());
        for (const RHIShaderResourceBinding& binding: totalBindings)
        {
            type_counts[VulkanDescriptorTypeGet(binding.Type)] += binding.Count;
        }

        Array<VkDescriptorPoolSize> poolSizes;
        for (const auto [type, count]: type_counts)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = type;
            poolSize.descriptorCount = count * static_cast<uint32_t>(maxGroupsCount);
            poolSizes.Append(poolSize);
        }

        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        createInfo.poolSizeCount = poolSizes.GetSize();
        createInfo.pPoolSizes = poolSizes.GetData();
        createInfo.maxSets = static_cast<uint32_t>(maxGroupsCount);

        WL_VULKAN_CHECK(VulkanAPI::vkCreateDescriptorPool(context.Device,
                                                          &createInfo,
                                                          context.Allocator,
                                                          &m_handle));
    }

    void VulkanShaderResourceGroupPool::Destroy()
    {
        for (size_t i = 0; i < GetCount(); i++)
        {
            if (RHIShaderResourceGroup* group = GetSRG(i))
            {
                DeallocateSRG(group);
            }
        }

        if (m_handle != VK_NULL_HANDLE)
        {
            VulkanContext& context = VulkanContextGet();
            VulkanAPI::vkDestroyDescriptorPool(context.Device, m_handle, context.Allocator);
            m_handle = VK_NULL_HANDLE;
            m_nextIndexToAllocate = 0;
        }
    }

}// namespace Wl
