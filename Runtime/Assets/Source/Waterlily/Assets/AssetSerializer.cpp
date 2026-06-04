#include "Waterlily/Assets/AssetSerializer.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    HashMap<StringID, AssetSerializer::SerializerEntry> AssetSerializer::s_serializers;

    void AssetSerializer::Serialize(OutputStream& stream, Asset& asset)
    {
        WL_CHECK(s_serializers.Contains(asset.AssetType));
        s_serializers[asset.AssetType].Serialize(stream, asset);
    }

    void AssetSerializer::Deserialize(InputStream& stream, Asset& asset)
    {
        WL_CHECK(s_serializers.Contains(asset.AssetType));
        s_serializers[asset.AssetType].Deserialize(stream, asset);
    }

}// namespace Wl
