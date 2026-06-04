#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{

    class FrameGraphTextureCacheKey;

    class FrameGraphPhysicalTexturePool
    {
    public:
        using Handle = size_t;

        Handle Obtain(const FrameGraphTextureCacheKey& key);

        void Release(Handle handle);

        FrameGraphPhysicalTexture& GetResource(Handle);

        void Destroy(Handle handle);

        void Destroy();

        FrameGraphPhysicalTexturePool(const SharedPtr<RHIDevice>& device)
            : m_device(device)
        {
        }

    private:
        FrameGraphPhysicalTexture Create(const FrameGraphTextureCacheKey& key);

    private:
        SharedPtr<RHIDevice> m_device;
        Array<FrameGraphPhysicalTexture> m_resources;
        Array<size_t> m_freeList;
    };

}// namespace Wl