#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/assetMetadata.hpp"
#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    AssetMetadata& AssetRegistry::GetMetadata(AssetHandle handle)
    {
        return m_registry[m_uuidToTndex[handle.UUID]];
    }

    AssetMetadata& AssetRegistry::GetMetadata(StringID uri)
    {
        return m_registry[m_uriToIndex[uri]];
    }

    AssetHandle AssetRegistry::GetAssetHandle(StringID uri)
    {
        return {GetMetadata(uri).GetUUID()};
    }

    void AssetRegistry::RegisterMetadata(const AssetMetadata& metadata)
    {
        if (!HasMetadata(metadata.GetURI()))
        {
            m_registry.Append(metadata);

            size_t index = m_registry.GetSize() - 1;
            m_uuidToTndex.Emplace(metadata.GetUUID(), index);
            m_uriToIndex.Emplace(metadata.GetURI(), index);
        }
    }

    bool AssetRegistry::HasMetadata(AssetHandle handle) const
    {
        return m_uuidToTndex.Contains(handle.UUID);
    }

    bool AssetRegistry::HasMetadata(StringID uri) const
    {
        return m_uriToIndex.Contains(uri);
    }

    AssetHandle AssetRegistry::CreateAsset(StringID assetType, StringID uri)
    {
        if (HasMetadata(uri))
        {
            AssetMetadata& metadata = GetMetadata(uri);
            return {metadata.GetUUID()};
        }

        AssetUUID UUID = AssetUUID::Generate();

        AssetMetadata metadata(UUID, assetType, uri);
        RegisterMetadata(metadata);

        return {UUID};
    }

    void AssetRegistry::AddDependency(AssetHandle parent, AssetHandle child)
    {
        if (!HasMetadata(parent) || !HasMetadata(child))
        {
            return;
        }

        AssetMetadata& parentMetadata = GetMetadata(parent);
        AssetMetadata& childMetadata = GetMetadata(child);

        parentMetadata.GetDependencies().Add(childMetadata.GetUUID());
    }

    SharedPtr<AssetRegistry> AssetRegistry::CreateFromFile(File& file)
    {
        SharedPtr<AssetRegistry> registry = MakeShared<AssetRegistry>();

        file.Seek(0);

        WLARHeader header;
        file << header;

        return registry;
    }

    SharedPtr<AssetRegistry> AssetRegistry::LoadFromFile(File& file)
    {
        SharedPtr<AssetRegistry> assetRegistry = MakeShared<AssetRegistry>();

        file.Seek(0);

        WLARHeader header;
        file >> header;

        if (header.Filetype != WLAR_FILETYPE)
        {
            return nullptr;
        }

        file.Seek(header.AssetOffset);
        for (uint64_t i = 0; i < header.AssetCount; i++)
        {
            AssetMetadata metadata;
            file >> metadata;
            assetRegistry->RegisterMetadata(metadata);
        }

        file.Seek(0);

        return assetRegistry;
    }

    void AssetRegistry::PersistFile(SharedPtr<AssetRegistry>& registry, File& file)
    {
        file.Seek(0);

        WLARHeader header;
        header.AssetCount = registry->m_registry.GetSize();
        file << header;

        file.Seek(header.AssetOffset);
        for (const AssetMetadata& metadata: registry->m_registry)
        {
            file << metadata;
        }
    }

}// namespace Wl