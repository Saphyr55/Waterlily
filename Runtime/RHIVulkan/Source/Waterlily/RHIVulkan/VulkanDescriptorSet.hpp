#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"

#include <vk_mem_alloc.h>

namespace Wl
{

    class VulkanShaderResourceGroup : public RHIShaderResourceGroup
    {
    public:
        virtual void SetBuffer(const RHIWriteBufferResource& resource) override;

        virtual void SetSampler(const RHIWriteSamplerResource& resource) override;

        virtual void SetTextureSampler(const RHIWriteTextureSamplerResource& resource) override;

        virtual void SetTexture(const RHIWriteTextureResource& resource) override;

        virtual void Update() override;

        inline uint32 SetIndexPool() const override
        {
            return m_indexPool;
        }

        inline VkDescriptorSet& GetHandle()
        {
            return m_handle;
        }

        inline void SetIndexPool(usize indexPool)
        {
            m_indexPool = indexPool;
        }

        inline void Reset()
        {
            SetIndexPool(0);
            m_handle = VK_NULL_HANDLE;
        }

        VulkanShaderResourceGroup() = default;
        virtual ~VulkanShaderResourceGroup() override = default;

    private:
        struct PendingBufferWrite
        {
            uint32 Binding;
            uint32 ArrayElement;
            VkBuffer Buffer;
            VkDeviceSize Offset;
            VkDeviceSize Range;
            VkDescriptorType Type;
        };

        struct PendingImageWrite
        {
            uint32 Binding;
            uint32 ArrayElement;
            VkSampler Sampler;
            VkImageView View;
            VkImageLayout Layout;
            VkDescriptorType Type;
        };

        Array<PendingBufferWrite> m_pendingBufferWrites;
        Array<PendingImageWrite> m_pendingImageWrites;

        VkDescriptorSet m_handle = VK_NULL_HANDLE;
        usize m_indexPool = 0;
    };

}// namespace Wl
