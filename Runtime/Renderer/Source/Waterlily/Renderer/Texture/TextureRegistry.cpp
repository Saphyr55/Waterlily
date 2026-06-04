#include "Waterlily/Renderer/Texture/TextureRegistry.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/DeviceMemoryUploader.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

namespace Wl
{

    TextureHandle TextureRegistry::ObtainTexture(AssetHandle asset, bool isNormalize)
    {
        if (!asset.IsValid())
        {
            return InvalidTexture;
        }

        if (m_handles.Contains(asset))
        {
            return m_handles[asset];
        }

        TextureHandle handle = m_count++;
        m_registry.Put(handle, Texture(nullptr, nullptr, nullptr, isNormalize));
        m_handles.Put(asset, handle);
        m_pendings.Emplace(asset, handle);

        return handle;
    }

    void TextureRegistry::Upload(bool doReleaseImage)
    {
        RHIDeviceMemoryUploader uploader(m_device);
        for (PendingUploadTexture& pending: m_pendings)
        {
            TextureAsset* imageAsset = m_assetManager.GetAsset<TextureAsset>(pending.Asset);
            Image& image = imageAsset->Image;

            Texture& texture = m_registry.Get(pending.Handle);

            RHITextureDescription textureDescription = {};
            textureDescription.Format = RHIFormatFromImage(image, texture.IsNormalize);
            textureDescription.MemoryUsage = RHIMemoryUsage::Device;
            textureDescription.Usage = RHITextureUsageFlags::Sampler;
            textureDescription.SharingMode = RHISharingMode::Shared;
            textureDescription.Width = image.Width;
            textureDescription.Height = image.Height;
            textureDescription.MipLevels =
                    Math::Floor(Math::Log2(Math::Max(textureDescription.Width, textureDescription.Height))) + 1;

            size_t sizeInBytes = image.GetSizeInBytes();

            texture.Texture =
                    uploader.SendTexture(RHIStagingBufferContext(RHIBufferUsageFlags::Storage, sizeInBytes, image.Data.GetData()),
                                         textureDescription);

            texture.View = m_device->CreateTextureView(RHITextureViewDescription{
                    .Texture = texture.Texture,
                    .Format = textureDescription.Format,
                    .Dimension = RHITextureDimension::Dim2D,
                    .MipLevels = textureDescription.MipLevels,
            });

            texture.Sampler = m_device->CreateSampler(RHISamplerDescription{
                    .MaxLOD = static_cast<float>(textureDescription.MipLevels),
            });

            if (doReleaseImage)
            {
                m_assetManager.Unload<TextureAsset>(pending.Asset);
            }
        }
        m_pendings.Clear();

        uploader.Upload(m_device->GetGraphicsQueue());
    }

    void TextureRegistry::CompileSRG()
    {
        for (auto [handle, data]: GetRegistry())
        {
            if (handle == TextureRegistry::InvalidTexture || data.View == nullptr || data.Sampler == nullptr)
            {
                continue;
            }
            RHIWriteTextureSamplerResource write(m_binding, data.View, data.Sampler, handle);
            m_group->SetTextureSampler(write);
        }
        m_group->Update();
    }

    void TextureRegistry::Dispose()
    {
        for (auto [handle, data]: GetRegistry())
        {
            m_device->DestroyTexture(data.Texture);
            m_device->DestroySampler(data.Sampler);
            m_device->DestroyTextureView(data.View);
        }

        m_device->DestroyTexture(m_dummyTexture.Texture);
        m_device->DestroySampler(m_dummyTexture.Sampler);
        m_device->DestroyTextureView(m_dummyTexture.View);
        m_device->DestroyBindlessShaderResources(m_bindlessResources);
    }

    void TextureRegistry::InitDummyTexture()
    {
        RHIDeviceMemoryUploader uploader(m_device);

        RHITextureDescription textureDescription = {};
        textureDescription.Format = RHIFormat::RGBA8sRGB;
        textureDescription.MemoryUsage = RHIMemoryUsage::Device;
        textureDescription.Usage = RHITextureUsageFlags::Sampler;
        textureDescription.SharingMode = RHISharingMode::Shared;
        textureDescription.Width = 2;
        textureDescription.Height = 2;
        textureDescription.MipLevels = 1;

        size_t textureSize = static_cast<size_t>(textureDescription.Width) * static_cast<size_t>(textureDescription.Height) * 4;

        Array<uint8_t> textureBuffer;
        textureBuffer.Resize(textureSize);
        for (size_t i = 0; i < textureSize; i++)
        {
            textureBuffer[i] = 255;
        }

        m_dummyTexture.Texture = uploader.SendTexture(
                RHIStagingBufferContext(RHIBufferUsageFlags::Storage, textureBuffer.GetSize(), textureBuffer.GetData()),
                textureDescription);

        m_dummyTexture.View = m_device->CreateTextureView({
                .Texture = m_dummyTexture.Texture,
                .Format = textureDescription.Format,
                .Dimension = RHITextureDimension::Dim2D,
                .MipLevels = textureDescription.MipLevels,
        });

        m_dummyTexture.Sampler = m_device->CreateSampler({});

        uploader.Upload(m_device->GetGraphicsQueue());
    }

    void TextureRegistry::InitSRG()
    {
        RHIShaderResourceBinding binding(m_binding,
                                         RHIShaderResourceType::CombinedTextureSampler,
                                         RHIShaderStage::Fragment,
                                         MaxResources);

        m_bindlessResources = m_device->CreateBindlessShaderResources(MaxResources, {binding});
        m_srgLayout = m_bindlessResources->GetSRGLayout();
        m_group = m_bindlessResources->GetSRG();
    }

}// namespace Wl
