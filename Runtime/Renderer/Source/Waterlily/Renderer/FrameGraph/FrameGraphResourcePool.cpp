#include "Waterlily/Renderer/FrameGraph/frameGraphResourcePool.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphCache.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{

    FrameGraphPhysicalTexture FrameGraphPhysicalTexturePool::Create(const FrameGraphTextureCacheKey& key)
    {
        FrameGraphPhysicalTexture physicalTexture;

        physicalTexture.Texture = m_device->CreateTexturre(RHITextureDescription{
                .Format = key.Format,
                .Usage = key.Usage,
                .SharingMode = RHISharingMode::Private,
                .MemoryUsage = RHIMemoryUsage::Device,
                .Width = key.Width,
                .Height = key.Height,
        });

        physicalTexture.View = m_device->CreateTextureView(RHITextureViewDescription{
                .Texture = physicalTexture.Texture,
                .Format = key.Format,
        });

        return physicalTexture;
    }

    FrameGraphPhysicalTexture& FrameGraphPhysicalTexturePool::GetResource(Handle handle)
    {
        return m_resources[handle];
    }

    FrameGraphPhysicalTexturePool::Handle FrameGraphPhysicalTexturePool::Obtain(const FrameGraphTextureCacheKey& key)
    {
        if (m_freeList.IsEmpty())
        {
            size_t index = m_resources.GetSize();
            FrameGraphPhysicalTexture& physicalResource = m_resources.Emplace();
            physicalResource = Create(key);
            return index;
        }

        size_t index = m_freeList.Back();
        m_freeList.Pop();
        return index;
    }

    void FrameGraphPhysicalTexturePool::Destroy(Handle handle)
    {
        FrameGraphPhysicalTexture& physicalResource = GetResource(handle);
        m_device->DestroyTexture(physicalResource.Texture);
        m_device->DestroyTextureView(physicalResource.View);
    }

    void FrameGraphPhysicalTexturePool::Destroy()
    {
        for (FrameGraphPhysicalTexture& physicalResource: m_resources)
        {
            m_device->DestroyTexture(physicalResource.Texture);
            m_device->DestroyTextureView(physicalResource.View);
        }
        m_freeList.Clear();
        m_resources.Clear();
    }

    void FrameGraphPhysicalTexturePool::Release(Handle handle)
    {
        m_freeList.push_back(handle);
    }

}// namespace Wl