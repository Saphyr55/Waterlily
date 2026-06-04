#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct Image
    {
        Array<uint8_t> Data;
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Channels = 0;

        inline size_t GetStride() const
        {
            return Width * Channels;
        }

        inline bool IsEmpty() const
        {
            return Data.IsEmpty() || Width == 0 || Height == 0 || Channels == 0;
        }

        inline size_t GetSizeInBytes() const
        {
            return Width * Height * Channels;
        }
    };

    inline RHIFormat RHIFormatFromImage(const Image& image, const bool normal = false)
    {
        RHIFormat format = RHIFormat::RGBA8sRGB;

        if (normal)
        {
            switch (image.Channels)
            {
                case 1:
                    format = RHIFormat::R8;
                    break;
                case 2:
                    format = RHIFormat::RG8;
                    break;
                case 3:
                    format = RHIFormat::RGB8;
                    break;
                case 4:
                    format = RHIFormat::RGBA8;
                    break;
                default:
                    LLOG_WARN("[Image]", Wl::Format("Unsupported image component count: %u", image.Channels));
                    break;
            }
        }
        else
        {
            switch (image.Channels)
            {
                case 1:
                    format = RHIFormat::R8sRGB;
                    break;
                case 2:
                    format = RHIFormat::RG8sRGB;
                    break;
                case 3:
                    format = RHIFormat::RGB8sRGB;
                    break;
                case 4:
                    format = RHIFormat::RGBA8sRGB;
                    break;
                default:
                    LLOG_WARN("[Image]", Wl::Format("Unsupported image component count: %u", image.Channels));
                    break;
            }
        }

        return format;
    }

}// namespace Wl