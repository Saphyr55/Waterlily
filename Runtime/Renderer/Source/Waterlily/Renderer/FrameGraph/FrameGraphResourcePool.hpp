#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include <cstdint>

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

        inline PooledPhysicalTexture& GetResource(PooledPhysicalTextureHandle handle)
        {
            return m_resources[handle];
        }

        PooledPhysicalTextureHandle Obtain(const FrameGraphPhysicalTextureKey& key, uint64_t currentFrame);

        void Release(const FrameGraphPhysicalTextureKey& key, PooledPhysicalTextureHandle handle);

        void GarbageCollect(uint64_t currentFrame, uint64_t maxFrameLifetime);

        void Dispose();

        FrameGraphPhysicalTexturePool(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
        }

    private:
        PooledPhysicalTexture& Allocate(const FrameGraphPhysicalTextureKey& key, uint64_t currentFrame);
        FrameGraphPhysicalTexture Create(const FrameGraphPhysicalTextureKey& key);
        void Destroy(PooledPhysicalTexture& handle);

    private:
        SharedPtr<RHIDevice> m_device;

        using KeyType = FrameGraphPhysicalTextureKey;
        HashMap<KeyType, Array<size_t>, FrameGraphPhysicalTextureKeyHash> m_freeList;

        Array<PooledPhysicalTexture> m_resources;
    };

}// namespace Wl