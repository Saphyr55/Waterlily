#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"


namespace Wl
{

    using PooledPhysicalTextureHandle = size_t;

    struct PooledPhysicalTexture
    {
        FrameGraphPhysicalTexture PhysicalTexture;
        uint64_t LastUsedFrame = 0;

        PooledPhysicalTexture(const FrameGraphPhysicalTexture& physicalTexture, uint64_t lastUsedFrame)
            : PhysicalTexture(physicalTexture)
            , LastUsedFrame(lastUsedFrame)
        {
        }
    };

    class FrameGraphPhysicalTexturePool
    {
    public:
        using Handle = size_t;

        void BeginFrame(uint64_t maxFrameLifetime);

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
            , m_allocator(MemoryStack::GetGlobalAllocator(), 64 * WL_KB)
        {
        }

    private:
        PooledPhysicalTexture& Allocate(const FrameGraphPhysicalTextureKey& key, uint64_t currentFrame);
        FrameGraphPhysicalTexture Create(const FrameGraphPhysicalTextureKey& key);
        void Destroy(PooledPhysicalTexture& handle);

        void GarbageCollect(uint64_t maxFrameLifetime);

    private:
        SharedPtr<RHIDevice> m_device;
        SharedPtr<FrameContext> m_frameContext;

        LinearAllocator m_allocator;
        HashMap<FrameGraphPhysicalTextureKey, Array<PooledPhysicalTextureHandle>, FrameGraphPhysicalTextureKeyHash> m_freeList;

        struct PendingRelease
        {
            FrameGraphPhysicalTextureKey Key;
            PooledPhysicalTextureHandle Handle;
        };
        Array<PendingRelease> m_pendingReleases;

        Array<PooledPhysicalTexture> m_resources;

        uint64_t m_frameCount = 0;
    };

}// namespace Wl