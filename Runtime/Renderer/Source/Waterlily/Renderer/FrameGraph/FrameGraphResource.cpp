#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{
    FrameGraphPhysicalTextureKey FrameGraphPhysicalTextureKey::Create(const FrameGraphTextureResource& resource)
    {
        FrameGraphPhysicalTextureKey key = {};
        key.Format = resource.Info.Format;
        key.Usage = resource.Usage;
        key.Width = resource.Info.Width;
        key.Height = resource.Info.Height;

        return key;
    }

    FrameGraphTextureResource FrameGraphResource::CreatePersistantResource(RHITexture* texture, RHITextureView* view)
    {
        FrameGraphTextureResource resource = {};
        resource.Info.Format = texture->GetDescription().Format;
        resource.Info.Width = texture->GetDescription().Width;
        resource.Info.Height = texture->GetDescription().Height;
        resource.Info.MipLevels = texture->GetDescription().MipLevels;
        resource.Info.Layers = texture->GetDescription().Layers;
        resource.Info.Levels = texture->GetDescription().MipLevels;
        resource.Usage = texture->GetDescription().Usage;
        resource.IsTransient = false;
        resource.PersistantResource.Texture = texture;
        resource.PersistantResource.View = view;

        return resource;
    }

    FrameGraphBufferResource FrameGraphResource::CreatePersistantResource(RHIBuffer* buffer, size_t size, size_t offset)
    {
        FrameGraphBufferResource resource = {};
        resource.Info.Size = size;
        resource.Info.Offset = offset;
        resource.PhysicalTexture.Handle = buffer;
        resource.Usage = buffer->GetUsage();
        resource.IsTransient = false;

        return resource;
    }

}// namespace Wl
