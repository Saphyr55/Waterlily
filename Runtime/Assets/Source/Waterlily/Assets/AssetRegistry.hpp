#pragma once

#include <unordered_map>

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/assetMetadata.hpp"
#include "Waterlily/Assets/assetsExports.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{
    inline constexpr StringRef WLAR_EXTENSION = ".wlar";
    inline constexpr uint64_t WLAR_FILETYPE = Wl::CStringPack64("WLAR    ");
    inline constexpr uint32_t WLAR_VERSION = 1;
    inline constexpr StringRef WLAR_FILENAME = "Registry.wlar";

    struct WLARHeader
    {
        uint64_t Filetype = WLAR_FILETYPE;
        uint32_t Version = WLAR_VERSION;
        uint64_t AssetCount = 0;
        uint64_t AssetOffset = sizeof(WLARHeader);
    };

    inline void operator<<(OutputStream& stream, const WLARHeader& header)
    {
        stream << header.Filetype;
        stream << header.Version;
        stream << header.AssetCount;
        stream << header.AssetOffset;
    }

    inline void operator>>(InputStream& stream, WLARHeader& header)
    {
        stream >> header.Filetype;
        stream >> header.Version;
        stream >> header.AssetCount;
        stream >> header.AssetOffset;
    }

    class WL_ASSETS_API AssetRegistry
    {
    public:
        static SharedPtr<AssetRegistry> CreateFromFile(File& file);
        static SharedPtr<AssetRegistry> LoadFromFile(File& file);
        static void PersistFile(SharedPtr<AssetRegistry>& registry, File& file);

    public:
        AssetHandle CreateAsset(StringID assetType, StringID uri);

        void AddDependency(AssetHandle parent, AssetHandle child);

        AssetMetadata& GetMetadata(AssetHandle handle);
        AssetMetadata& GetMetadata(StringID uri);

        AssetHandle GetAssetHandle(StringID uri);

        void RegisterMetadata(const AssetMetadata& metadata);

        bool HasMetadata(AssetHandle handle) const;
        bool HasMetadata(StringID uri) const;

        inline ArrayView<AssetMetadata> GetRegistry() const
        {
            return m_registry;
        }

    public:
        AssetRegistry() = default;
        ~AssetRegistry() = default;

    private:
        HashMap<StringID, size_t> m_uriToIndex;
        HashMap<AssetUUID, size_t> m_uuidToTndex;
        Array<AssetMetadata> m_registry;
    };

}// namespace Wl
