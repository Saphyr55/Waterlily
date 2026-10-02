#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/RHIExports.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Resource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{
    inline static usize s_countTextureAllocation = 0;

    struct RHITextureLayoutTransition
    {
        RHITexture* Texture = nullptr;
        RHITextureLayout OldLayout = RHITextureLayout::Undefined;
        RHITextureLayout NewLayout = RHITextureLayout::Undefined;
    };

    struct RHITextureDescription
    {
        RHIFormat Format = RHIFormat::RGBA32_FLOAT;
        RHITextureUsageFlags Usage = RHITextureUsageFlags::ColorAttachment;
        RHISharingMode SharingMode = RHISharingMode::Private;
        RHIMemoryUsage MemoryUsage = RHIMemoryUsage::Device;
        RHITextureDimension Dimension = RHITextureDimension::Dim2D;
        RHITextureLayout Layout = RHITextureLayout::Undefined;
        uint32 Width = 8;
        uint32 Height = 8;
        usize Depth = 1;
        uint32 MipLevels = 1;
        usize Layers = 1;
    };

    class WL_RHI_API RHITexture : public RHIResource
    {
    public:
        const RHITextureDescription& GetDescription() const;

        virtual void Bind() = 0;

        virtual ~RHITexture() = default;

    protected:
        RHITexture() = default;

    protected:
        RHITextureDescription m_description;
    };

    WL_RHI_API void RHITransitionTexture(SharedPtr<RHIDevice> device,
                                         const RHITextureLayoutTransition& barrier,
                                         const SharedPtr<RHICommandQueue>& queue);

}// namespace Wl
