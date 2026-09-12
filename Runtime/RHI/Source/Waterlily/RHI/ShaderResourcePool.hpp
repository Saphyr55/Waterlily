#pragma once

#include "Waterlily/RHI/ShaderResource.hpp"

namespace Wl
{

    class RHIShaderResourceGroupPool
    {
    public:
        virtual RHIShaderResourceGroup* AllocateSRG(RHIShaderResourceGroupLayout* layout) = 0;

        virtual void DeallocateSRG(RHIShaderResourceGroup* srg) = 0;

        virtual void Reset() = 0;

        virtual RHIShaderResourceGroup* GetSRG(size_t groupIndex) = 0;

        virtual size_t GetCount() = 0;

        virtual uint32_t GetMaxCount() = 0;

        virtual ~RHIShaderResourceGroupPool() = default;
    };

}// namespace Wl
