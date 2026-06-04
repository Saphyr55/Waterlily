#pragma once

#include "Waterlily/RHI/Sampler.hpp"
#include "Waterlily/RHI/Texture.hpp"

namespace Wl
{

    struct Texture
    {
        RHITexture* Texture;
        RHITextureView* View;
        RHISampler* Sampler;
        bool IsNormalize;
    };

}// namespace Wl