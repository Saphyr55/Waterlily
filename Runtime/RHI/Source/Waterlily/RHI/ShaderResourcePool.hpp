#pragma once

#include "Waterlily/RHI/ShaderResource.hpp"

namespace Wl
{

    /**
 * @brief Base class for managing the allocation and deallocation of Shader Resource Groups (Vulkan example:
 * VkDescriptorPool).
 */
    class RHIShaderResourceGroupPool
    {
    public:
        /**
     * @brief Allocates a new Shader Resource Group based on the provided layout.
     */
        virtual RHIShaderResourceGroup* AllocateSRG(RHIShaderResourceGroupLayout* layout) = 0;

        /**
     * @brief Returns a Shader Resource Group to the pool for reuse.
     */
        virtual void DeallocateSRG(RHIShaderResourceGroup* srg) = 0;

        virtual void Reset() = 0;

        /**
     * @brief Retrieves a Shader Resource Group by its internal pool index (e.g., for pre-allocated pools).
     */
        virtual RHIShaderResourceGroup* GetSRG(size_t groupIndex) = 0;

        virtual size_t GetCount() = 0;

        virtual size_t GetMaxCount() = 0;

        virtual ~RHIShaderResourceGroupPool() = default;
    };

}// namespace Wl
