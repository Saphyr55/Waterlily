#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

#include <cstddef>

namespace Wl
{

    FrameGraphTextureResource FrameGraphResource::FromRHI(RHITexture* texture, RHITextureView* view)
    {
        return FrameGraphTextureResource{
                .Info =
                        {
                                .Format = texture->GetDescription().Format,
                                .Width = texture->GetDescription().Width,
                                .Height = texture->GetDescription().Height,
                                .MipLevels = texture->GetDescription().MipLevels,
                                .Layers = texture->GetDescription().Layers,
                                .Levels = texture->GetDescription().MipLevels,
                        },
                .PhysicalTexture =
                        {
                                .Texture = texture,
                                .View = view,
                        },
                .Usage = texture->GetDescription().Usage,
                .IsTransient = false,
        };
    }

    FrameGraphBufferResource FrameGraphResource::FromRHI(RHIBuffer* buffer, size_t size, size_t offset)
    {
        return FrameGraphBufferResource{
                .Info =
                        {
                                .Size = size,
                                .Offset = offset},
                .PhysicalTexture =
                        {
                                .Handle = buffer,
                        },
                .Usage = buffer->GetUsage(),
                .IsTransient = false,
        };
    }

}// namespace Wl
