#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"


namespace Wl
{

    using PooledPhysicalTextureHandle = usize;

    struct PooledPhysicalTexture
    {
        FrameGraphPhysicalTexture PhysicalTexture;
        uint64 LastUsedFrame = 0;

        PooledPhysicalTexture(const FrameGraphPhysicalTexture& physicalTexture, uint64 lastUsedFrame)
            : PhysicalTexture(physicalTexture)
            , LastUsedFrame(lastUsedFrame)
        {
        }
    };

    class FrameGraphPhysicalTexturePool
    {
    public:
        using Handle = usize;

        void GarbageCollect(uint64 maxFrameLifetime);

        inline PooledPhysicalTexture& GetResource(PooledPhysicalTextureHandle handle)
        {
            return m_resources[handle];
        }

        PooledPhysicalTextureHandle Obtain(const FrameGraphPhysicalTextureKey& key);

        void Release(const FrameGraphPhysicalTextureKey& key, PooledPhysicalTextureHandle handle);

        void Dispose();

        FrameGraphPhysicalTexturePool(const SharedPtr<FrameContext>& frameContext)
            : m_device(frameContext->GetDevice())
            , m_frameContext(frameContext)
            , m_allocator(MemoryStack::GetGlobalAllocator(), 16 * WL_KB)
            , m_freeList(*MemoryStack::GetGlobalAllocator())
            , m_pendingReleases(*MemoryStack::GetGlobalAllocator())
            , m_resources(*MemoryStack::GetGlobalAllocator())
        {
        }

    private:
        PooledPhysicalTexture& Allocate(const FrameGraphPhysicalTextureKey& key, uint64 currentFrame);
        FrameGraphPhysicalTexture Create(const FrameGraphPhysicalTextureKey& key);
        void Destroy(PooledPhysicalTexture& handle);

        void InternalGarbageCollect(uint64 maxFrameLifetime);

    private:
        struct PendingRelease
        {
            FrameGraphPhysicalTextureKey Key;
            PooledPhysicalTextureHandle Handle;
        };

        SharedPtr<RHIDevice> m_device;
        SharedPtr<FrameContext> m_frameContext;

        LinearAllocator m_allocator;
        HashMap<FrameGraphPhysicalTextureKey, Array<PooledPhysicalTextureHandle>, FrameGraphPhysicalTextureKeyHash> m_freeList;

        Array<PendingRelease> m_pendingReleases;
        Array<PooledPhysicalTexture> m_resources;

        uint64 m_frameCount = 0;
    };

}// namespace Wl