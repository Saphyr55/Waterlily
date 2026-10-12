#pragma once

#include "Waterlily/Core/Containers/Handler.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
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
        StringRef Name;
        RHIFormat Format;
        SizeClass SizeClass = SizeClass::Swapchain;
        uint32 Width = 1'024;
        uint32 Height = 1'024;
        uint32 MipLevels = 1;
        usize Layers = 1;
        usize Levels = 1;
    };

    struct FrameGraphPhysicalTexture
    {
        RHITexture* Texture = nullptr;
        RHITextureView* View = nullptr;
    };

    struct FrameGraphBufferInfo
    {
        StringRef Name;
        usize Size;
        usize Offset = 0;
    };

    struct FrameGraphPhysicalBuffer
    {
        RHIBuffer* Handle;
    };

    struct FrameGraphResourceLifetime
    {
        usize FirstUse = UINT32_MAX;
        usize LastUse = 0;
    };

    struct FrameGraphTextureResource
    {
        FrameGraphTextureInfo Info;
        usize PooledResource;
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
        uint32 Width = 1;
        uint32 Height = 1;

        static FrameGraphPhysicalTextureKey Create(const FrameGraphTextureResource& resource);

        bool operator==(const FrameGraphPhysicalTextureKey& other) const noexcept
        {
            return Format == other.Format
                && Usage == other.Usage 
                && Width == other.Width 
                && Height == other.Height;
        }
    };

    class FrameGraphPhysicalTextureKeyHash
    {
    public:
        inline uint32 operator()(const FrameGraphPhysicalTextureKey& key) const noexcept
        {
            uint32 h = Hash<uint32>(uint32(key.Format));
            h = HashCombine(h, key.Width);
            h = HashCombine(h, key.Width);
            h = HashCombine(h, key.Height);
            h = HashCombine(h, uint32(key.Usage));
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
        static FrameGraphBufferResource CreatePersistantResource(RHIBuffer* buffer, usize size, usize offset);
    };


}// namespace Wl