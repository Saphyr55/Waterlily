#pragma once

#include "Waterlily/Core/Containers/Handler.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/TextureView.hpp"
#include "Waterlily/RHI/Types.hpp"

#include <cstddef>

namespace Wl
{

    using FrameGraphTextureHandle = Handler<RHITexture>;
    using FrameGraphBufferHandle = Handler<RHIBuffer>;

    enum class SizeClass
    {
        Absolute,
        Swapchain
    };

    struct FrameGraphTextureInfo
    {
        String Name;
        RHIFormat Format;
        SizeClass SizeClass = SizeClass::Swapchain;
        uint32_t Width = 1'024;
        uint32_t Height = 1'024;
        uint32_t MipLevels = 1;
        size_t Layers = 1;
        size_t Levels = 1;
    };

    struct FrameGraphPhysicalTexture
    {
        RHITexture* Texture = nullptr;
        RHITextureView* View = nullptr;
    };

    struct FrameGraphBufferInfo
    {
        String Name;
        size_t Size;
        size_t Offset = 0;
    };

    struct FrameGraphPhysicalBuffer
    {
        RHIBuffer* Handle;
    };

    struct FrameGraphResourceLifetime
    {
        size_t FirstUse = UINT32_MAX;
        size_t LastUse = 0;
    };

    struct FrameGraphTextureResource
    {
        FrameGraphTextureInfo Info;
        size_t PooledResource;
        FrameGraphPhysicalTexture PersistantResource;
        RHITextureUsageFlags Usage = RHITextureUsageFlags::None;
        RHITextureLayout CurrentLayout = RHITextureLayout::Undefined;
        FrameGraphResourceLifetime Lifetime;
        bool IsTransient = false;
        bool IsAllocated = false;
    };

    struct FrameGraphPhysicalTextureKey
    {
        RHIFormat Format;
        RHITextureUsageFlags Usage;
        uint32_t Width = 1;
        uint32_t Height = 1;

        static FrameGraphPhysicalTextureKey Create(const FrameGraphTextureResource& resource);

        bool operator==(const FrameGraphPhysicalTextureKey& other) const noexcept
        {
            return Format == other.Format && Usage == other.Usage && Width == other.Width && Height == other.Height;
        }
    };

    class FrameGraphPhysicalTextureKeyHash
    {
    public:
        inline uint32_t operator()(const FrameGraphPhysicalTextureKey& key) const noexcept
        {
            uint32_t h = Hash<uint32_t>()(uint32_t(key.Format));
            h = HashCombine(h, key.Width);
            h = HashCombine(h, key.Width);
            h = HashCombine(h, key.Height);
            h = HashCombine(h, uint32_t(key.Usage));
            return h;
        }
    };

    struct FrameGraphBufferResource
    {
        FrameGraphBufferInfo Info;
        FrameGraphPhysicalBuffer PhysicalBuffer;
        RHIBufferUsageFlags Usage;
        FrameGraphResourceLifetime Lifetime;
        bool IsTransient = false;
        bool IsAllocated = false;
    };

    struct FrameGraphTextureBarrier
    {
        FrameGraphTextureHandle Handle;
        RHITextureLayout OldLayout;
        RHITextureLayout NewLayout;
    };

    class FrameGraphResource
    {
    public:
        static FrameGraphTextureResource CreatePersistantResource(RHITexture* texture, RHITextureView* view);
        static FrameGraphBufferResource CreatePersistantResource(RHIBuffer* buffer, size_t size, size_t offset);
    };


}// namespace Wl