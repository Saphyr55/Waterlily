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

        virtual RHIShaderResourceGroup* GetSRG(usize poolIndex) override;

        virtual void Reset() override;

        virtual usize GetCount() override
        {
            return m_allocatedGroups.size();
        }

        virtual uint32 GetMaxCount() override
        {
            return m_maxGroupsCount;
        }

        void Create(uint32 maxGroups, const Array<RHIShaderResourceBinding>& totalBindings);
        void Destroy();

        VulkanShaderResourceGroupPool() = default;
        virtual ~VulkanShaderResourceGroupPool() override = default;

    private:
        HashSet<usize> m_freeGroups;
        Array<VulkanShaderResourceGroup> m_allocatedGroups;
        VkDescriptorPool m_handle = VK_NULL_HANDLE;
        uint32 m_maxGroupsCount = 0;
        usize m_nextIndexToAllocate = 0;
    };

}// namespace Wl
