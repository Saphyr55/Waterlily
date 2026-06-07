#include "Waterlily/RHIVulkan/VulkanDescriptorSet.hpp"

#include "Waterlily/RHIVulkan/VulkanBuffer.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanSampler.hpp"
#include "Waterlily/RHIVulkan/VulkanTextureView.hpp"
#include "Waterlily/RHIVulkan/VulkanTexture.hpp"

namespace Wl
{

    void VulkanShaderResourceGroup::SetBuffer(const RHIWriteBufferResource& resource)
    {
        VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(resource.Buffer);
        WL_CHECK(vulkanBuffer);
        
        bool isStorageBuffer = (vulkanBuffer->GetUsage() & RHIBufferUsageFlags::Storage) == RHIBufferUsageFlags::Storage;

        PendingBufferWrite pendingWrite = {};
        pendingWrite.ArrayElement = static_cast<uint32_t>(resource.ArrayIndex);
        pendingWrite.Binding = resource.Binding;
        pendingWrite.Buffer = vulkanBuffer->GetHandle();
        pendingWrite.Offset = resource.Offset;
        pendingWrite.Range = resource.Range;
        pendingWrite.Type = isStorageBuffer ? VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        m_pendingBufferWrites.Append(pendingWrite);
    }

    void VulkanShaderResourceGroup::SetSampler(const RHIWriteSamplerResource& /*resource*/)
    {
        WL_CRASH("Not implemented.");
    }

    void VulkanShaderResourceGroup::SetTextureSampler(const RHIWriteTextureSamplerResource& resource)
    {
        VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(resource.TextureView);
        VulkanSampler* vulkanSampler = static_cast<VulkanSampler*>(resource.Sampler);
        WL_CHECK(vulkanTextureView && vulkanSampler);
        
        VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(vulkanTextureView->GetDescription().Texture);
        WL_CHECK(vulkanTexture);

        RHITextureUsageFlags usage = vulkanTexture->GetDescription().Usage;
        bool isSampled = (usage & RHITextureUsageFlags::Sampler) == RHITextureUsageFlags::Sampler;
        WL_CHECK_MSG(isSampled, "Only sampled textures can be bound with a sampler.");

        PendingImageWrite pendingWrite = {};
        pendingWrite.ArrayElement = static_cast<uint32_t>(resource.ArrayIndex);
        pendingWrite.Binding = resource.Binding;
        pendingWrite.View = vulkanTextureView->GetHandle();
        pendingWrite.Sampler = vulkanSampler->GetHandle();
        pendingWrite.Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        pendingWrite.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        m_pendingImageWrites.Append(pendingWrite);
    }

    void VulkanShaderResourceGroup::SetTexture(const RHIWriteTextureResource& resource)
    {
        VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(resource.TextureView);
        WL_CHECK(vulkanTextureView);

        VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(vulkanTextureView->GetDescription().Texture);
        WL_CHECK(vulkanTexture);

        RHITextureUsageFlags usage = vulkanTexture->GetDescription().Usage;
        bool isStorage = (usage & RHITextureUsageFlags::Storage) == RHITextureUsageFlags::Storage;
        
        WL_CHECK_MSG(isStorage, "Only storage textures or read-only texture views can be bound without a sampler.");

        PendingImageWrite pendingWrite = {};
        pendingWrite.ArrayElement = static_cast<uint32_t>(resource.ArrayIndex);
        pendingWrite.Binding = resource.Binding;
        pendingWrite.View = vulkanTextureView->GetHandle();
        pendingWrite.Sampler = VK_NULL_HANDLE;
        pendingWrite.Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        pendingWrite.Type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

        m_pendingImageWrites.Append(pendingWrite);
    }

    void VulkanShaderResourceGroup::Update()
    {
        const size_t bufferCount = m_pendingBufferWrites.GetSize();
        const size_t imageCount = m_pendingImageWrites.GetSize();

        if (bufferCount + imageCount == 0)
        {
            return;
        }

        Array<VkDescriptorBufferInfo> bufferInfos;
        bufferInfos.Reserve(bufferCount);

        Array<VkDescriptorImageInfo> imageInfos;
        imageInfos.Reserve(imageCount);

        Array<VkWriteDescriptorSet> writes;
        writes.Reserve(bufferCount + imageCount);

        for (uint32_t i = 0; i < bufferCount; i++)
        {
            PendingBufferWrite& pending = m_pendingBufferWrites[i];
            
            bufferInfos.Append(VkDescriptorBufferInfo {
                    .buffer = pending.Buffer,
                    .offset = pending.Offset,
                    .range = pending.Range,
            });

            writes.Append(VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_handle,
                    .dstBinding = pending.Binding,
                    .dstArrayElement = pending.ArrayElement,
                    .descriptorCount = 1,
                    .descriptorType = pending.Type,
                    .pBufferInfo = &bufferInfos[i],
            });
        }

        for (uint32_t i = 0; i < imageCount; i++)
        {
            PendingImageWrite& pending = m_pendingImageWrites[i];

            imageInfos.Append(VkDescriptorImageInfo {
                    .sampler = pending.Sampler,
                    .imageView = pending.View,
                    .imageLayout = pending.Layout,
            });

            writes.Append(VkWriteDescriptorSet {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_handle,
                    .dstBinding = pending.Binding,
                    .dstArrayElement = pending.ArrayElement,
                    .descriptorCount = 1,
                    .descriptorType = pending.Type,
                    .pImageInfo = &imageInfos[i],
            });
        }

        VulkanContext& context = VulkanContextGet();

        VulkanAPI::vkUpdateDescriptorSets(context.Device,
                                          static_cast<uint32_t>(writes.GetSize()), 
                                          writes.GetData(),
                                          0,
                                          nullptr);

        m_pendingBufferWrites.Clear();
        m_pendingImageWrites.Clear();
    }

}// namespace Wl