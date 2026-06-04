#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/RHIExports.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Resource.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

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
        uint32_t Width = 8;
        uint32_t Height = 8;
        size_t Depth = 1;
        uint32_t MipLevels = 1;
        size_t Layers = 1;
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
