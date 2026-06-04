#pragma once

#include "Waterlily/RHI/Pipeline.hpp"

namespace Wl
{

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