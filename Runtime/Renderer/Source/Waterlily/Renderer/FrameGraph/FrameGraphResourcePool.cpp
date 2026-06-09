#include "Waterlily/Renderer/FrameGraph/FrameGraphResourcePool.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    void FrameGraphPhysicalTexturePool::BeginFrame(uint64_t maxFrameLifetime)
    {
        if (maxFrameLifetime < m_frameContext->GetMaxFrameInFlight())
        {
            maxFrameLifetime = maxFrameLifetime + m_frameContext->GetMaxFrameInFlight();
        }
        
        m_frameCount = m_frameContext->GetFrameCount();
        GarbageCollect(maxFrameLifetime);
         
        for (const PendingRelease& pendingRelease: m_pendingReleases)
        {
            m_freeList[pendingRelease.Key].Append(pendingRelease.Handle);
        }
        m_pendingReleases.Clear();
    }

    PooledPhysicalTextureHandle FrameGraphPhysicalTexturePool::Obtain(const FrameGraphPhysicalTextureKey& key)
    {
        if (Array<PooledPhysicalTextureHandle>* pooledResources = m_freeList.GetPtr(key))
        {
            PooledPhysicalTextureHandle pooledResourceHandle = pooledResources->Back();
            pooledResources->PopBack();
            m_resources[pooledResourceHandle].LastUsedFrame = m_frameCount;
            return pooledResourceHandle;
        }
        
        Allocate(key, m_frameCount);
        
        return m_resources.GetSize() - 1;
    }

    void FrameGraphPhysicalTexturePool::Release(const FrameGraphPhysicalTextureKey& key, PooledPhysicalTextureHandle pooledResource)
    {
        m_pendingReleases.Emplace(key, pooledResource);
    }

    void FrameGraphPhysicalTexturePool::Dispose()
    {
        for (PooledPhysicalTexture& pooledResource: m_resources)
        {
            Destroy(pooledResource);
        }
        m_freeList.Clear();
        m_resources.Clear();
    }

    void FrameGraphPhysicalTexturePool::GarbageCollect(uint64_t maxFrameLifetime)
    {
        // TODO: 
    }
    
    PooledPhysicalTexture& FrameGraphPhysicalTexturePool::Allocate(const FrameGraphPhysicalTextureKey& key, uint64_t currentFrame)
    {
        return m_resources.Emplace(Create(key), currentFrame);
    }

    FrameGraphPhysicalTexture FrameGraphPhysicalTexturePool::Create(const FrameGraphPhysicalTextureKey& key)
    {
        FrameGraphPhysicalTexture physicalTexture;

        physicalTexture.Texture = m_device->CreateTexturre(RHITextureDescription {
                .Format = key.Format,
                .Usage = key.Usage,
                .SharingMode = RHISharingMode::Private,
                .MemoryUsage = RHIMemoryUsage::Device,
                .Width = key.Width,
                .Height = key.Height,
        });

        physicalTexture.View = m_device->CreateTextureView(RHITextureViewDescription {
                .Texture = physicalTexture.Texture,
                .Format = key.Format,
        });

        return physicalTexture;
    }

    void FrameGraphPhysicalTexturePool::Destroy(PooledPhysicalTexture& pooledResource)
    {
        m_device->DestroyTexture(pooledResource.PhysicalTexture.Texture);
        m_device->DestroyTextureView(pooledResource.PhysicalTexture.View);
    }

}// namespace Wl