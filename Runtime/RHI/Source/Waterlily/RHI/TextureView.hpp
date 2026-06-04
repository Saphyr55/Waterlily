#pragma once

#include "Waterlily/RHI/RHIExports.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Resource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct RHITextureViewDescription
    {
        RHITexture* Texture = nullptr;
        RHIFormat Format = RHIFormat::RGBA8sRGB;
        RHITextureDimension Dimension = RHITextureDimension::Dim2D;
        uint32_t MipLevels = 1;
    };

    class WL_RHI_API RHITextureView : public RHIResource
    {
    public:
        const RHITextureViewDescription& GetDescription() const
        {
            return m_description;
        }

        virtual ~RHITextureView() = default;

    protected:
        RHITextureView()
        {
        }

    protected:
        RHITextureViewDescription m_description;
    };

}// namespace Wl