#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include <cstddef>

namespace Wl
{

    struct FrameGraphPhysicalTextureKey
    {
        RHIFormat Format;
        RHITextureUsageFlags Usage;
        uint32_t Width = 1;
        uint32_t Height = 1;

        bool operator==(const FrameGraphPhysicalTextureKey& other) const noexcept
        {
            return Format == other.Format && Width == other.Width && Height == other.Height;
        }
    };

    class FrameGraphPhysicalTextureHash
    {
    public:
        inline size_t operator()(const FrameGraphPhysicalTextureKey& key) const noexcept
        {
            RHIFormat format = key.Format;
            size_t h = Hash<uint32_t>()(uint32_t(format));
            h ^= Hash<uint32_t>()(key.Width);
            h ^= Hash<uint32_t>()(key.Height);
            return h;
        }
    };

    struct PooledPhysicalTexture
    {
        FrameGraphPhysicalTexture PhysicalTexture;
        size_t LastUsedFrame = 0;
    };

    class FrameGraphPhysicalTexturePool
    {
    public:
        PooledPhysicalTexture* Obtain(const FrameGraphTextureCacheKey& key, size_t currentFrame);

        void Release(PooledPhysicalTexture* handle);

        void Dispose();

        FrameGraphPhysicalTexturePool(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
        }

    private:
        FrameGraphPhysicalTexture Create(const FrameGraphTextureCacheKey& key);
        void Destroy(Handle handle);

    private:
        SharedPtr<RHIDevice> m_device;
        Array<FrameGraphPhysicalTexture> m_resources;
        Array<size_t> m_freeList;
    };

}// namespace Wl