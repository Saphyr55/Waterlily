#pragma once

#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHISamplerDescription
    {
        RHIFilter MagFilter = RHIFilter::LINEAR;
        RHIFilter MinFilter = RHIFilter::LINEAR;
        RHISamplerAddressMode AddressModeU = RHISamplerAddressMode::Repeat;
        RHISamplerAddressMode AddressModeV = RHISamplerAddressMode::Repeat;
        RHISamplerAddressMode AddressModeW = RHISamplerAddressMode::Repeat;
        float MaxLOD = 0.0f;
        float MinLOD = 0.0f;
        bool IsNormalize = false;
    };

    class RHISampler
    {
    public:
        inline const RHISamplerDescription& GetDescription() const
        {
            return m_description;
        }

    public:
        virtual ~RHISampler() = default;

    protected:
        RHISampler() = default;

    protected:
        RHISamplerDescription m_description;
    };

}// namespace Wl