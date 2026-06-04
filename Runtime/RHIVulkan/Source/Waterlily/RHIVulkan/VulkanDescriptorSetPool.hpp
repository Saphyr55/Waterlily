#pragma once

#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourcePool.hpp"
#include "Waterlily/RHIVulkan/VulkanDescriptorSet.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanShaderResourceGroupPool : public RHIShaderResourceGroupPool
    {
    public:
        virtual RHIShaderResourceGroup* AllocateSRG(RHIShaderResourceGroupLayout* layout) override;
        virtual void DeallocateSRG(RHIShaderResourceGroup* group) override;

        virtual RHIShaderResourceGroup* GetSRG(size_t pool_index) override;

        virtual void Reset() override;

        virtual size_t GetCount() override
        {
            return m_allocatedGroups.size();
        }

        virtual size_t GetMaxCount() override
        {
            return m_maxGroupsCount;
        }

        void Create(size_t maxGroups, const Array<RHIShaderResourceBinding>& totalBindings);
        void Destroy();

        VulkanShaderResourceGroupPool() = default;
        virtual ~VulkanShaderResourceGroupPool() override = default;

    private:
        HashSet<size_t> m_freeGroups;
        Array<VulkanShaderResourceGroup> m_allocatedGroups;
        VkDescriptorPool m_handle = VK_NULL_HANDLE;
        size_t m_maxGroupsCount = 0;
        size_t m_nextIndexToAllocate = 0;
    };

}// namespace Wl
