#pragma once

#include "Waterlily/RHI/BindlessShaderResources.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSet.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSetLayout.hpp"

namespace Wl
{

    class VulkanBindlessShaderResources : public RHIBindlessShaderResources
    {
    public:
        virtual RHIShaderResourceGroup* GetSRG() override
        {
            return &m_srg;
        }

        virtual RHIShaderResourceGroupLayout* GetSRGLayout() override
        {
            return &m_layout;
        }

        virtual uint32_t GetMaxResources() override
        {
            return m_maxResources;
        }

        virtual void Create(uint32_t max_resources, const Array<RHIShaderResourceBinding>& bindings) override;

        virtual void Destroy() override;

        VulkanBindlessShaderResources() = default;
        virtual ~VulkanBindlessShaderResources() = default;

    private:
        VkDescriptorSetLayoutBinding BindingSetMapping(const RHIShaderResourceBinding& binding);

    private:
        VulkanShaderResourceGroup m_srg;
        VulkanShaderResourceGroupLayout m_layout;
        VkDescriptorPool m_pool;
        uint32_t m_maxResources = 0;
    };

}// namespace Wl
