#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    class AssetMetadata
    {
    public:
        inline AssetUUID GetUUID() const
        {
            return m_uuid;
        }

        inline StringID GetAssetType() const
        {
            return m_assetType;
        }

        inline StringID GetURI() const
        {
            return m_uri;
        }

        inline const HashSet<AssetUUID>& GetDependencies() const
        {
            return m_deps;
        }

        inline HashSet<AssetUUID>& GetDependencies()
        {
            return m_deps;
        }

    public:
        AssetMetadata() = default;

        AssetMetadata(AssetUUID UUID, const StringID& assetType, const StringID& uri)
            : m_uuid(UUID)
            , m_assetType(assetType)
            , m_uri(uri)
        {
        }

    private:
        AssetUUID m_uuid = AssetUUID::CreateInvalid();
        StringID m_assetType;
        StringID m_uri;
        HashSet<AssetUUID> m_deps;
    };

    inline void operator<<(OutputStream& stream, const AssetMetadata& metadata)
    {
        stream << metadata.GetUUID();
        stream << metadata.GetAssetType();
        stream << metadata.GetURI();
        stream << static_cast<uint64_t>(metadata.GetDependencies().GetSize());
        for (const AssetUUID& UUID: metadata.GetDependencies())
        {
            stream << UUID;
        }
    }

    inline void operator>>(InputStream& stream, AssetMetadata& metadata)
    {
        AssetUUID UUID = AssetUUID::CreateInvalid();
        StringID assetType;
        StringID conditionedURI;
        uint64_t depsCount = 0;

        stream >> UUID;
        stream >> assetType;
        stream >> conditionedURI;
        stream >> depsCount;

        metadata = AssetMetadata(UUID, assetType, conditionedURI);

        for (uint64_t i = 0; i < depsCount; i++)
        {
            AssetUUID depUUID;
            stream >> depUUID;
            metadata.GetDependencies().Add(depUUID);
        }
    }

}// namespace Wl
