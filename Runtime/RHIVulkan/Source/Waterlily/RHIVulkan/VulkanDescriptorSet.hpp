#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
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

        inline uint32_t GetIndexPool() const override
        {
            return m_indexPool;
        }

        inline VkDescriptorSet& GetHandle()
        {
            return m_handle;
        }

        inline void Init(size_t indexPool)
        {
            GetIndexPool(indexPool);
        }

        inline void GetIndexPool(size_t indexPool)
        {
            m_indexPool = indexPool;
        }

        inline void Reset()
        {
            Init(0);
            m_handle = VK_NULL_HANDLE;
        }

        VulkanShaderResourceGroup() = default;
        virtual ~VulkanShaderResourceGroup() override = default;

    private:
        struct PendingBufferWrite
        {
            uint32_t Binding;
            uint32_t ArrayElement;
            VkBuffer Buffer;
            VkDeviceSize Offset;
            VkDeviceSize Range;
            VkDescriptorType Type;
        };

        struct PendingImageWrite
        {
            uint32_t Binding;
            uint32_t ArrayElement;
            VkSampler Sampler;
            VkImageView View;
            VkImageLayout Layout;
            VkDescriptorType Type;
        };

        Array<PendingBufferWrite> m_pendingBufferWrites;
        Array<PendingImageWrite> m_pendingImageWrites;

        VkDescriptorSet m_handle = VK_NULL_HANDLE;
        size_t m_indexPool = 0;
    };

}// namespace Wl
