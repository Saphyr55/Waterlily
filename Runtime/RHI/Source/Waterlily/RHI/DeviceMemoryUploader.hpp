#pragma once

#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/BufferPool.hpp"
#include "Waterlily/RHI/RHIExports.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    struct WL_RHI_API RHIStagingBufferContext
    {
        RHIBufferUsageFlags Usage;
        size_t Size;     // Size in bytes;
        const void* Data;// Data to transfer to the device.

        RHIStagingBufferContext(RHIBufferUsageFlags usage, size_t sizeInBytes, void* data)
            : Usage(usage)
            , Size(sizeInBytes)
            , Data(data)
        {
        }

        template<typename T>
        RHIStagingBufferContext(RHIBufferUsageFlags usage, const ArrayView<T>& data)
            : Usage(usage)
            , Size(data.GetSizeInBytes())
            , Data(data.GetData())
        {
        }

        ~RHIStagingBufferContext() = default;
    };

    class WL_RHI_API RHIDeviceMemoryUploader
    {
    public:
        RHIBuffer* SendBuffer(const RHIStagingBufferContext& context);

        RHIBuffer* SendBuffer(RHIBufferUsageFlags usage, size_t size, void* data)
        {
            return SendBuffer(RHIStagingBufferContext(usage, size, data));
        }

        template<typename T>
        RHIBuffer* SendBuffer(RHIBufferUsageFlags usage, const ArrayView<T>& data)
        {
            return SendBuffer(RHIStagingBufferContext(usage, data));
        }

        RHITexture* SendTexture(const RHIStagingBufferContext& context, RHITextureDescription& description);

        void Upload(const SharedPtr<RHICommandQueue>& queue);

        RHIDeviceMemoryUploader(SharedPtr<RHIDevice> device,
                                size_t capacity = 8);

    private:
        RHIBufferDescription CreateStagingBufferDescription(const RHIStagingBufferContext& context);
        RHIBufferDescription CreateBufferDescription(const RHIStagingBufferContext& context);

    private:
        struct BufferEntry
        {
            RHIBuffer* Staging;
            RHIBuffer* Buffer;
            size_t Size;
        };

        struct TextureEntry
        {
            RHIBuffer* Staging;
            RHITexture* Texture;
            size_t Size;
        };

        Array<RHIBuffer*> m_stangings;
        SharedPtr<RHIDevice> m_device;
        Array<BufferEntry> m_bufferEntries;
        Array<TextureEntry> m_textureEntries;
    };

}// namespace Wl