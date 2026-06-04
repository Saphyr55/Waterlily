#include "Waterlily/RHIVulkan/VulkanDescriptorSet.hpp"

#include "Waterlily/RHIVulkan/VulkanBuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanSampler.hpp"
#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"

namespace Wl
{

    void VulkanShaderResourceGroup::SetBuffer(const RHIWriteBufferResource& resource)
    {
        VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(resource.Buffer);
        VkDescriptorType descriptorType = VulkanDescriptorTypeGet(m_layout->GetResourceType(resource.Binding));

        PendingDescriptorArray& entry = m_pendings[resource.Binding];
        entry.Type = descriptorType;

        if (entry.BufferInfos.GetSize() <= resource.ArrayIndex)
        {
            if (entry.BufferInfos.IsEmpty())
            {
                entry.BufferInfos = {};
            }
            entry.BufferInfos.Resize(resource.ArrayIndex + 1);
        }

        VkDescriptorBufferInfo& info = entry.BufferInfos[resource.ArrayIndex];
        info.buffer = vulkanBuffer->GetHandle();
        info.offset = static_cast<VkDeviceSize>(resource.Offset);
        info.range = static_cast<VkDeviceSize>(resource.Range);
    }

    void VulkanShaderResourceGroup::SetSampler(const RHIWriteSamplerResource& /*resource*/)
    {
        WL_CRASH("Not implemented.")
    }

    void VulkanShaderResourceGroup::SetTextureSampler(const RHIWriteTextureSamplerResource& resource)
    {
        VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(resource.TextureView);
        VulkanSampler* vulkanSampler = static_cast<VulkanSampler*>(resource.Sampler);

        VkDescriptorType descriptorType = VulkanDescriptorTypeGet(m_layout->GetResourceType(resource.Binding));

        PendingDescriptorArray& entry = m_pendings[resource.Binding];
        entry.Type = descriptorType;

        if (entry.ImageInfos.GetSize() <= resource.ArrayIndex)
        {
            if (entry.ImageInfos.IsEmpty())
            {
                entry.ImageInfos = {};
            }
            entry.ImageInfos.Resize(resource.ArrayIndex + 1);
        }

        VkDescriptorImageInfo& info = entry.ImageInfos[resource.ArrayIndex];
        info.sampler = vulkanSampler->GetHandle();
        info.imageView = vulkanTextureView->GetHandle();
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    void VulkanShaderResourceGroup::SetTexture(const RHIWriteTextureResource& /*resource*/)
    {
        WL_CRASH("Not implemented.")
    }

    void VulkanShaderResourceGroup::Update()
    {
        VulkanContext& context = VulkanContextGet();

        Array<VkWriteDescriptorSet> writes(m_pendings.GetSize());

        for (auto [binding, entry]: m_pendings)
        {
            if (!entry.BufferInfos.empty())
            {
                VkWriteDescriptorSet write{};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = m_handle;
                write.dstBinding = binding;
                write.descriptorType = entry.Type;
                write.descriptorCount = entry.BufferInfos.size();
                write.pBufferInfo = entry.BufferInfos.data();
                writes.Append(write);
            }

            if (!entry.ImageInfos.empty())
            {
                VkWriteDescriptorSet write{};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = m_handle;
                write.dstBinding = binding;
                write.descriptorType = entry.Type;
                write.descriptorCount = entry.ImageInfos.size();
                write.pImageInfo = entry.ImageInfos.data();
                writes.Append(write);
            }
        }

        if (!writes.empty())
        {
            VulkanAPI::vkUpdateDescriptorSets(context.Device, writes.size(), writes.data(), 0, nullptr);
        }
    }

}// namespace Wl