#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Renderer/Image.hpp"

namespace Wl
{

    inline const StringID AssetType_Texture2D = WL_SID("Texture2D");

    struct TextureAsset : Asset
    {
        Image Image;

        TextureAsset()
            : Asset(AssetType_Texture2D)
        {
        }

        virtual ~TextureAsset() override = default;
    };

    inline void operator<<(OutputStream& stream, const TextureAsset& asset)
    {
        stream << asset.Image.Width;
        stream << asset.Image.Height;
        stream << asset.Image.Channels;
        stream << asset.Image.Data.GetSize();
        stream.Write(asset.Image.Data.GetData(), asset.Image.Data.GetSize());
    }

    inline void operator>>(InputStream& stream, TextureAsset& asset)
    {
        uint64_t size = 0;
        stream >> asset.Image.Width;
        stream >> asset.Image.Height;
        stream >> asset.Image.Channels;
        stream >> size;
        asset.Image.Data.Resize(size);
        stream.Read(asset.Image.Data.GetData(), asset.Image.Data.GetSize());
    }

}// namespace Wl