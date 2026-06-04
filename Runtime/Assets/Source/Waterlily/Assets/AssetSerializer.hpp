#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/assetsExports.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class WL_ASSETS_API AssetSerializer
    {
    public:
        using SerializeFn = Function<void(OutputStream&, Asset&)>;
        using DeserializeFn = Function<void(InputStream&, Asset&)>;

        struct SerializerEntry
        {
            SerializeFn Serialize;
            DeserializeFn Deserialize;
        };

    public:
        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        static void RegisterAssetType(StringID assetType)
        {
            s_serializers.Put(
                    assetType,
                    {&AssetSerializer::SerializeImpl<AssetType>, &AssetSerializer::DeserializeImpl<AssetType>});
        }

        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        static void UnregisterAssetType(StringID assetType)
        {
            s_serializers.Remove(assetType);
        }

        static void Serialize(OutputStream& stream, Asset& asset);
        static void Deserialize(InputStream& stream, Asset& asset);

    private:
        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        static void SerializeImpl(OutputStream& stream, Asset& asset)
        {
            stream << static_cast<AssetType&>(asset);
        }

        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        static void DeserializeImpl(InputStream& stream, Asset& asset)
        {
            stream >> static_cast<AssetType&>(asset);
        }

    private:
        static HashMap<StringID, SerializerEntry> s_serializers;
    };

}// namespace Wl
