#pragma once

#include "FrameGraphResourcePool.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/ObjectCache.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"

namespace Wl
{

    struct FrameGraphTextureCacheKey
    {
        RHIFormat Format;
        RHITextureUsageFlags Usage;
        uint32_t Width = 1;
        uint32_t Height = 1;

        bool operator==(const FrameGraphTextureCacheKey& other) const noexcept
        {
            return Format == other.Format && Width == other.Width && Height == other.Height;
        }
    };

    class FrameGraphTextureResourceHash
    {
    public:
        inline size_t operator()(const FrameGraphTextureCacheKey& key) const noexcept
        {
            RHIFormat format = key.Format;
            size_t h = Hash<uint32_t>()(uint32_t(format));
            h ^= Hash<uint32_t>()(key.Width);
            h ^= Hash<uint32_t>()(key.Height);
            return h;
        }
    };

    class FrameGraphFramebufferCache : public ObjectCache<RHIFramebufferDescription, RHIFramebuffer*>
    {
    public:
        FrameGraphFramebufferCache(SharedPtr<RHIDevice> device)
        {
            m_createCallback = [device](const RHIFramebufferDescription& description) mutable -> RHIFramebuffer*
            {
                return device->CreateFramebuffer(description);
            };

            m_destroyCallback = [device](RHIFramebuffer*& framebuffer) mutable -> void
            {
                device->DestroyFramebuffer(framebuffer);
            };
        }
    };

    class FrameGraphRenderPassCache
    {
    public:
        RHIRenderPass* GetRenderPass(StringID name);
        RHIRenderPass* Create(StringID name, const RHIRenderPassDescription& description);
        void Clear();

        FrameGraphRenderPassCache(SharedPtr<RHIDevice> device)
            : m_device(device)
        {
        }

    private:
        SharedPtr<RHIDevice> m_device;
        HashMap<StringID, RHIRenderPass*> m_renderPasses;
    };

    class FrameGraphPhysicalTextureCache
    {
    public:
        using CacheType =
                HashMap<FrameGraphTextureCacheKey, FrameGraphPhysicalTexturePool::Handle, FrameGraphTextureResourceHash>;

        FrameGraphPhysicalTexturePool::Handle Obtain(const FrameGraphTextureCacheKey& key)
        {
            auto it = m_cache.find(key);
            if (it != m_cache.end())
            {
                return (*it).Value;
            }

            FrameGraphPhysicalTexturePool::Handle handle = m_pool.Obtain(key);
            m_cache[key] = handle;
            m_reverseCache[handle] = key;
            return handle;
        }

        void Release(FrameGraphPhysicalTexturePool::Handle handle)
        {
            m_pool.Release(handle);
        }

        void Invalidate(const FrameGraphPhysicalTexturePool::Handle& handle)
        {
            FrameGraphTextureCacheKey key = m_reverseCache[handle];
            m_cache.Remove(key);
            m_reverseCache.Remove(handle);
        }

        void Clear()
        {
            m_reverseCache.Clear();
            m_cache.Clear();
        }

        FrameGraphPhysicalTextureCache(FrameGraphPhysicalTexturePool& pool, const SharedPtr<RHIDevice>& device)
            : m_pool(pool)
            , m_device(device)
        {
        }

    private:
        HashMap<FrameGraphPhysicalTexturePool::Handle, FrameGraphTextureCacheKey> m_reverseCache;
        FrameGraphPhysicalTexturePool& m_pool;
        SharedPtr<RHIDevice> m_device;
        CacheType m_cache;
    };

}// namespace Wl
