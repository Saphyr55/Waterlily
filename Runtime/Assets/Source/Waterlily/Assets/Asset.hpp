#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Identifiers/UUID.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    using AssetUUID = UUID_64;

    struct AssetHandle
    {
        AssetUUID UUID = AssetUUID::CreateInvalid();

        bool operator==(AssetHandle other) const
        {
            return other.UUID == UUID;
        }

        bool operator!=(AssetHandle other) const
        {
            return other.UUID != UUID;
        }

        bool IsValid()
        {
            return UUID.IsValid();
        }

        AssetHandle() = default;
        AssetHandle(AssetUUID uuid)
            : UUID(uuid)
        {
        }
        ~AssetHandle() = default;
    };

    inline void operator<<(OutputStream& stream, const AssetHandle& handle)
    {
        stream << handle.UUID;
    }

    inline void operator>>(InputStream& stream, AssetHandle& handle)
    {
        stream >> handle.UUID;
    }

    struct AssetDependency
    {
        StringID AssetType;
        StringID URI;
    };

    struct Asset
    {
        StringID AssetType;

        Asset(const StringID& assetType)
            : AssetType(assetType)
        {
        }

        virtual ~Asset() = default;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::AssetHandle, h, { return Wl::Hash<uint64_t>()(h.UUID.GetValue()); })
