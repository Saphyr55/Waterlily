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

        inline void Init(size_t indexPool, RHIShaderResourceGroupLayout* layout)
        {
            GetIndexPool(indexPool);
            SetLayout(layout);
        }

        inline void GetIndexPool(size_t indexPool)
        {
            m_indexPool = indexPool;
        }

        inline void SetLayout(RHIShaderResourceGroupLayout* layout)
        {
            m_layout = layout;
        }

        inline void Reset()
        {
            Init(0, nullptr);
            m_handle = VK_NULL_HANDLE;
        }

        VulkanShaderResourceGroup() = default;
        virtual ~VulkanShaderResourceGroup() override = default;

    private:
        struct PendingDescriptorArray
        {
            VkDescriptorType Type;
            Array<VkDescriptorBufferInfo> BufferInfos;
            Array<VkDescriptorImageInfo> ImageInfos;
        };

        HashMap<uint32_t, PendingDescriptorArray> m_pendings;

        RHIShaderResourceGroupLayout* m_layout = nullptr;
        VkDescriptorSet m_handle = VK_NULL_HANDLE;
        size_t m_indexPool = 0;
    };

}// namespace Wl
