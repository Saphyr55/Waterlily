#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetsExports.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class WL_ASSETS_API AssetSerializer
    {
    public:
        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        inline static void RegisterAssetType(StringID assetType)
        {
            GetInstance().m_serializers.Put(
                    assetType,
                    {&AssetSerializer::SerializeImpl<AssetType>, &AssetSerializer::DeserializeImpl<AssetType>});
        }

        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        inline static void UnregisterAssetType(StringID assetType)
        {
            GetInstance().m_serializers.Remove(assetType);
        }

        static void Serialize(OutputStream& stream, Asset& asset);
        static void Deserialize(InputStream& stream, Asset& asset);

    private:
        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        inline static void SerializeImpl(OutputStream& stream, Asset& asset)
        {
            stream << static_cast<AssetType&>(asset);
        }

        template<typename AssetType>
            requires std::derived_from<AssetType, Asset>
        inline static void DeserializeImpl(InputStream& stream, Asset& asset)
        {
            stream >> static_cast<AssetType&>(asset);
        }

    private:
        using SerializeFn = Function<void(OutputStream&, Asset&)>;
        using DeserializeFn = Function<void(InputStream&, Asset&)>;

        struct SerializerEntry
        {
            SerializeFn Serialize;
            DeserializeFn Deserialize;
        };

        static AssetSerializer& GetInstance();

        HashMap<StringID, SerializerEntry> m_serializers;
    };

}// namespace Wl
