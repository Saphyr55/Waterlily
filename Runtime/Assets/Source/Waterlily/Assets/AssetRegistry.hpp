#pragma once

#include <unordered_map>

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/assetMetadata.hpp"
#include "Waterlily/Assets/assetsExports.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/IO/FileHandle.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{
    inline constexpr StringRef LAR_EXTENSION = ".lar";
    inline constexpr uint64_t LAR_FILETYPE = Wl::CStringPack64("LAR     ");
    inline constexpr uint32_t LAR_VERSION = 1;
    inline constexpr StringRef LAR_FILENAME = "Registry.lar";

    struct LARHeader
    {
        uint64_t Filetype = LAR_FILETYPE;
        uint32_t Version = LAR_VERSION;
        uint64_t AssetCount = 0;
        uint64_t AssetOffset = sizeof(LARHeader);
    };

    inline void operator<<(OutputStream& stream, const LARHeader& header)
    {
        stream << header.Filetype;
        stream << header.Version;
        stream << header.AssetCount;
        stream << header.AssetOffset;
    }

    inline void operator>>(InputStream& stream, LARHeader& header)
    {
        stream >> header.Filetype;
        stream >> header.Version;
        stream >> header.AssetCount;
        stream >> header.AssetOffset;
    }

    class WL_ASSETS_API AssetRegistry
    {
    public:
        static SharedPtr<AssetRegistry> CreateFromFile(FileHandle& file);
        static SharedPtr<AssetRegistry> LoadFromFile(FileHandle& file);
        static void PersistFile(SharedPtr<AssetRegistry>& registry, FileHandle& file);

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
