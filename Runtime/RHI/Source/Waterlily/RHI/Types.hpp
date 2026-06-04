#pragma once

#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    struct Viewport
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Width = 0.0f;
        float Height = 0.0f;
        float MinDepth = 0.0f;
        float MaxDepth = 1.0f;

        bool operator==(const Viewport& other) const
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height &&
                   MinDepth == other.MinDepth && MaxDepth == other.MaxDepth;
        }
    };

    struct Rect2D
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Width = 0.0f;
        float Height = 0.0f;

        bool operator==(const Rect2D& other) const
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }
    };

    enum class RHIAttachmentLoadOp
    {
        Load,
        Clear,
        DontCare,
        None,
    };

    enum class RHIAttachmentStoreOp
    {
        Store,
        DontCare,
        None,
    };

    enum class RHIFilter
    {
        NEAREST,
        LINEAR
    };

    enum class RHISamplerAddressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class RHITextureLayout : uint8_t
    {
        Undefined,
        TransferDst,
        TransferSrc,
        ShaderReadOnly,
        ColorAttachment,
        DepthStencilAttachment,
        Present,
        General
    };

    enum class RHIShaderResourceType : uint8_t
    {
        Uniform,
        Sampler,
        CombinedTextureSampler,
        StorageBuffer,
        StorageTexture
    };

    enum class RHIQueueType : uint8_t
    {
        Unknown = 0,
        Graphics = 1 << 0,
        Compute = 1 << 1,
        Transfer = 1 << 2,
    };
    WL_ENUM_FLAGS(RHIQueueType)

    enum class RHIVertexInputRate
    {
        Vertex,
        Instance,
    };

    enum class RHIShaderStage : uint8_t
    {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Geometry = 1 << 2,
        Tesselation = 1 << 3,
        Compute = 1 << 4,
        AllGraphics = Vertex | Fragment | Geometry | Tesselation
    };
    WL_ENUM_FLAGS(RHIShaderStage)

    enum class RHICullModeFlags : uint8_t
    {
        None = 0,
        Front = 1 << 0,
        Back = 1 << 1,
    };
    WL_ENUM_FLAGS(RHICullModeFlags)

    enum class RHICommandBufferUsageFlags : uint8_t
    {
        None = 0,
        OneTimeSubmit = 1 << 0,
        RenderPassContinue = 1 << 1,
        SimultaneousUse = 1 << 2,
    };
    WL_ENUM_FLAGS(RHICommandBufferUsageFlags)

    enum class RHIBufferUsageFlags : uint32_t
    {
        None = 0,
        Vertex = 1 << 0,
        Index = 1 << 1,
        Uniform = 1 << 2,
        Storage = 1 << 3,
        TransferSrc = 1 << 4,
        TransferDst = 1 << 5,
        Indirect = 1 << 6
    };
    WL_ENUM_FLAGS(RHIBufferUsageFlags)

    enum class RHITextureUsageFlags : uint8_t
    {
        None = 0,
        ColorAttachment = 1 << 0,
        DepthStencilAttachment = 1 << 1,
        Sampler = 1 << 2,
        Storage = 1 << 3,
        TransferSrc = 1 << 4,
        TransferDst = 1 << 5
    };
    WL_ENUM_FLAGS(RHITextureUsageFlags)

    enum class RHITextureDimension
    {
        Dim1D,
        Dim2D,
        Dim3D,
    };

    enum class RHIMemoryUsage
    {
        Device,
        Host,
    };

    enum class RHISharingMode
    {
        Private,
        Shared
    };

    enum class RHIFormat : uint32_t
    {
        Undefined,

        // 8-bit UNORM
        R8,
        RG8,
        RGB8,
        RGBA8,
        BGRA8,
        R8sRGB,
        RG8sRGB,
        RGB8sRGB,
        RGBA8sRGB,
        BGRA8sRGB,

        // 16-bit
        R16_SINT,
        RG16_SINT,
        RGB16_SINT,
        RGBA16_SINT,
        R16_UINT,
        RG16_UINT,
        RGB16_UINT,
        RGBA16_UINT,
        R16_FLOAT,
        RG16_FLOAT,
        RGB16_FLOAT,
        RGBA16_FLOAT,

        // 32-bit
        R32_UINT,
        RG32_UINT,
        RGB32_UINT,
        RGBA32_UINT,
        R32_SINT,
        RG32_SINT,
        RGB32_SINT,
        RGBA32_SINT,
        R32_FLOAT,
        RG32_FLOAT,
        RGB32_FLOAT,
        RGBA32_FLOAT,

        // Packed
        RGB10A2,
        R11G11B10_FLOAT,
        RGB9E5,

        // Depth / Stencil
        D16,
        D24,
        D32,
        D24S8,
        D32S8,

        // Compressed (examples)
        BC1,
        BC3,
        BC5,
        BC7,
        ETC2_RGB8,
        ETC2_RGBA8,
        ASTC_4x4,
        ASTC_8x8,
    };

    inline bool RHIFormatIsDepth(const RHIFormat format)
    {
        switch (format)
        {
            case RHIFormat::D16:
            case RHIFormat::D24:
            case RHIFormat::D32:
            case RHIFormat::D24S8:
            case RHIFormat::D32S8:
                return true;

            default:
                return false;
        }
    }

    inline bool RHIFormatIsDepthStencil(RHIFormat format)
    {
        switch (format)
        {
            case RHIFormat::D24S8:
            case RHIFormat::D32S8:
                return true;

            default:
                return false;
        }
    }

    inline bool RHICommandBufferUsageHasFlag(RHICommandBufferUsageFlags value, RHICommandBufferUsageFlags flag)
    {
        return (value & flag) == flag;
    }

    inline bool RHIBufferUsageAny(RHIBufferUsageFlags usage)
    {
        return static_cast<uint8_t>(usage) != 0;
    }

}// namespace Wl