#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"

namespace Wl
{

    class RHIBindlessShaderResources
    {
    public:
        virtual RHIShaderResourceGroup* GetSRG() = 0;

        virtual RHIShaderResourceGroupLayout* GetSRGLayout() = 0;

        virtual uint32_t GetMaxResources() = 0;

        virtual void Create(uint32_t maxResources, const Array<RHIShaderResourceBinding>& bindings) = 0;

        virtual void Destroy() = 0;

        RHIBindlessShaderResources() = default;
        virtual ~RHIBindlessShaderResources() = default;
    };

}// namespace Wl
