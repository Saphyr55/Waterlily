#include "ACP/Texture/TextureAssetImporter.hpp"
#include "ACP/AssetImporter.hpp"
#include "ACP/PathConvertion.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/MemoryStream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/Texture/TextureAsset.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Wl
{

    struct stbi_uc_Deleter
    {
        void operator()(stbi_uc* resource) noexcept
        {
            Destroy(resource);
        }

        void Destroy(stbi_uc* resource) noexcept
        {
            stbi_image_free(resource);
        }
    };

    SharedPtr<Asset> TextureAssetImporter::ImportAsset(ImportContext& context)
    {
        AssetRegistry& registry = *context.Registry;
        AssetStorage& storage = context.Storage;

        SharedPtr<Stream> stream = context.Source->OpenAsset(context.URI);
        if (!stream)
        {
            return nullptr;
        }

        Array<uint8_t> buffer;
        buffer.Resize(stream->GetSize());
        stream->Read(buffer.GetData(), buffer.GetSize());

        int32_t width = 0;
        int32_t height = 0;
        int32_t channels = 0;
        int32_t reqComp = STBI_rgb_alpha;
        SharedPtr<stbi_uc> data_guard(
                stbi_load_from_memory(buffer.GetData(), static_cast<int32_t>(buffer.GetSize()), &width, &height, &channels, reqComp),
                stbi_uc_Deleter());

        if (width <= 0 || height <= 0)
        {
            return nullptr;
        }

        if (!data_guard)
        {
            return nullptr;
        }

        uint32_t dataByteCount = static_cast<uint32_t>(width * height * reqComp);
        uint32_t textureChannels = static_cast<uint32_t>(reqComp);
        uint32_t textureWidth = static_cast<uint32_t>(width);
        uint32_t textureHeight = static_cast<uint32_t>(height);

        SharedPtr<TextureAsset> assetData = MakeShared<TextureAsset>();
        assetData->Image.Channels = textureChannels;
        assetData->Image.Width = textureWidth;
        assetData->Image.Height = textureHeight;
        assetData->Image.Data.Resize(dataByteCount);

        MemoryStream memoryStream(assetData->Image.Data.GetData(), assetData->Image.Data.GetSize());
        memoryStream.Write(data_guard.GetResource(), assetData->Image.Data.GetSize());

        String lcaPath = PathToLCAFilepath(context.URI, context.OutputURI);
        StringID sid = CreateSID(lcaPath);

        AssetHandle textureHandle = registry.CreateAsset(assetData->AssetType, sid);

        storage.Put(textureHandle, assetData);

        return assetData;
    }

}// namespace Wl