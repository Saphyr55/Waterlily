#pragma once

#include "Waterlily/RHI/Pipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHIComputePipelineDescription
    {
        RHIPipelineShaderStageCreateInfo ComputeShaderInfo = {RHIShaderStage::Compute};
        Array<RHIShaderResourceGroupLayout*> SRGLayouts;
        Array<RHIShaderConstantRange> ShaderConstantRanges;
    };

    class RHIComputePipeline : public RHIPipeline
    {
    public:
        virtual Type GetType() override final
        {
            return Type::Compute;
        }

        virtual ~RHIComputePipeline() = default;
    };

}// namespace Wl