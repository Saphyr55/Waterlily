#pragma once

#include "Waterlily/RHI/ShaderResource.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanShaderResourceGroupLayout : public RHIShaderResourceGroupLayout
    {
    public:
        VkDescriptorSetLayout& GetHandle()
        {
            return m_handle;
        }

        virtual const Array<RHIShaderResourceBinding>& GetBindings() const override
        {
            return m_bindings;
        }

        virtual RHIShaderResourceType GetResourceType(size_t binding) const override;

        void Create(const Array<RHIShaderResourceBinding>& bindings);
        void Destroy();

        inline void SetBindings(const Array<RHIShaderResourceBinding>& bindings)
        {
            m_bindings = bindings;
        }

    public:
        VulkanShaderResourceGroupLayout() = default;
        ~VulkanShaderResourceGroupLayout() = default;

    private:
        Array<RHIShaderResourceBinding> m_bindings;
        VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
    };

}// namespace Wl
