#include "Waterlily/Assets/AssetSerializer.hpp"

namespace Wl
{
    

    void AssetSerializer::Serialize(OutputStream& stream, Asset& asset)
    {
        AssetSerializer& instance = GetInstance();
        WL_CHECK(instance.m_serializers.Contains(asset.AssetType));
        instance.m_serializers[asset.AssetType].Serialize(stream, asset);
    }

    void AssetSerializer::Deserialize(InputStream& stream, Asset& asset)
    {
        AssetSerializer& instance = GetInstance();
        WL_CHECK(instance.m_serializers.Contains(asset.AssetType));
        instance.m_serializers[asset.AssetType].Deserialize(stream, asset);
    }

    AssetSerializer& AssetSerializer::GetInstance()
    {
        static AssetSerializer s_instance;
        return s_instance;
    }

}// namespace Wl
